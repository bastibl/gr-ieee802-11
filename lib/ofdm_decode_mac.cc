/*
 * Copyright (C) 2013 Bastian Bloessl <bloessl@ccs-labs.org>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include <ieee802-11/ofdm_decode_mac.h>

#include "utils.h"

#include <boost/crc.hpp>
#include <gnuradio/io_signature.h>
#include <itpp/itcomm.h>
#include <iomanip>


static __m128i metric0[4] __attribute__ ((aligned(16)));
static __m128i metric1[4] __attribute__ ((aligned(16)));
static __m128i path0[4] __attribute__ ((aligned(16)));
static __m128i path1[4] __attribute__ ((aligned(16)));
struct viterbi_state state0[64];
struct viterbi_state state1[64];
static int d_init = 0;

int decoded_count;

using namespace gr::ieee802_11;
using namespace itpp;

class ofdm_decode_mac_impl : public ofdm_decode_mac {

public:
ofdm_decode_mac_impl(bool log, bool debug) : block("ofdm_decode_mac",
			gr::io_signature::make(1, 1, 48 * sizeof(gr_complex)),
			gr::io_signature::make(0, 0, 0)),
			d_log(log),
			d_debug(debug),
			d_ofdm(BPSK_1_2),
			d_tx(d_ofdm, 0),
			d_frame_complete(true) {

	message_port_register_out(pmt::mp("out"));

}

int general_work (int noutput_items, gr_vector_int& ninput_items,
		gr_vector_const_void_star& input_items,
		gr_vector_void_star& output_items) {

	const gr_complex *in = (const gr_complex*)input_items[0];

	int i = 0;

	std::vector<gr::tag_t> tags;
	const uint64_t nread = this->nitems_read(0);

	dout << "Decode MAC: input " << ninput_items[0] << std::endl;

	while(i < ninput_items[0]) {

		get_tags_in_range(tags, 0, nread + i, nread + i + 1,
			pmt::string_to_symbol("ofdm_start"));

		if(tags.size()) {
			if (d_frame_complete == false) {
				dout << "Warning: starting to receive new frame before old frame was complete" << std::endl;
				dout << "Already copied " << copied << " out of " << d_tx.n_sym << " symbols of last frame" << std::endl;
			}
			d_frame_complete = false;

			pmt::pmt_t tuple = tags[0].value;
			int len_data = pmt::to_uint64(pmt::car(tuple));
			int encoding = pmt::to_uint64(pmt::cdr(tuple));

			ofdm_param ofdm = ofdm_param((Encoding)encoding);
			tx_param tx = tx_param(ofdm, len_data);

			// sanity check, more than MAX_SYM may indicate garbage
			if(tx.n_sym <= MAX_SYM && tx.n_encoded_bits <= MAX_BITS) {
				d_ofdm = ofdm;
				d_tx = tx;
				copied = 0;
				dout << "Decode MAC: frame start -- len " << len_data
					<< "  symbols " << tx.n_sym << "  encoding "
					<< encoding << std::endl;
			} else {
				dout << "Dropping frame which is too large (symbols or bits)" << std::endl;
			}
		}

		if(copied < d_tx.n_sym) {
			dout << "copy one symbol, copied " << copied << " out of " << d_tx.n_sym << std::endl;
			std::memcpy(sym + (copied * 48), in, 48 * sizeof(gr_complex));
			copied++;

			if(copied == d_tx.n_sym) {
				dout << "received complete frame - decoding" << std::endl;
				decode();
				in += 48;
				i++;
				d_frame_complete = true;
				break;
			}
		}

		in += 48;
		i++;
	}

	consume(0, i);
	return 0;
}

void decode() {
	demodulate();
	deinterleave();
	decode_conv();
	descramble();
	print_output();

	// skip service field
	boost::crc_32_type result;
	result.process_bytes(out_bytes + 2, d_tx.psdu_size);
	if(result.checksum() != 558161692) {
		dout << "checksum wrong -- dropping" << std::endl;
		return;
	}

	mylog(boost::format("encoding: %1% - length: %2% - symbols: %3%")
			% d_ofdm.encoding % d_tx.psdu_size % d_tx.n_sym);

	// create PDU
	pmt::pmt_t blob = pmt::make_blob(out_bytes + 2, d_tx.psdu_size - 4);
	pmt::pmt_t enc = pmt::from_uint64(d_ofdm.encoding);
	pmt::pmt_t dict = pmt::make_dict();
	dict = pmt::dict_add(dict, pmt::mp("encoding"), enc);
	message_port_pub(pmt::mp("out"), pmt::cons(dict, blob));
}

/***** Auxiliary functions used for demodulation *****/
// BPSK
bvec demodulate_bpsk(const cvec &signal) const
{

  bvec out(signal.size());
  out.set_size(signal.size(), false);

  for (int i = 0; i < signal.length(); i++) {
    out(i) = bin(signal(i).real() > 0);
  }

  return out;
}

// QPSK
bvec demodulate_qpsk(const cvec &signal) const
{

  int k = 2;

  bvec out(k * signal.size());
  out.set_size(k * signal.size(), false);

  for (int i = 0; i < signal.size(); i++) {
  	out(k*i) = bin(signal(i).real() > 0);
  	out(k*i + 1) = bin(signal(i).imag() > 0);
  }

  return out;
}

// 16QAM
bvec demodulate_16qam(const cvec &signal) const
{

  int k = 4;

  bvec out(k * signal.size());
  out.set_size(k * signal.size(), false);

  for (int i = 0; i < signal.size(); i++) {
  	out(k*i) = bin(signal(i).real() > 0);
  	out(k*i + 1) = bin(std::abs(signal(i).real()) < 0.63245);
  	out(k*i + 2) = bin(signal(i).imag() > 0);
  	out(k*i + 3) = bin(std::abs(signal(i).imag()) < 0.63245);
  }

  return out;
}

// 64QAM
bvec demodulate_64qam(const cvec &signal) const
{

  int k = 6;

  bvec out(k * signal.size());
  out.set_size(k * signal.size(), false);

  for (int i = 0; i < signal.size(); i++) {
  	out(k*i) = bin(signal(i).real() > 0);
  	out(k*i + 1) = bin(std::abs(signal(i).real()) < 0.6172);
  	out(k*i + 2) = bin(std::abs(signal(i).real()) < 0.9258 && std::abs(signal(i).real()) > 0.3086);
  	out(k*i + 3) = bin(signal(i).imag() > 0);
  	out(k*i + 4) = bin(std::abs(signal(i).imag()) < 0.6172);
  	out(k*i + 5) = bin(std::abs(signal(i).imag()) < 0.9258 && std::abs(signal(i).imag()) > 0.3086);
  }

  return out;
}


void demodulate() {

	cvec symbols;
	symbols.set_length(d_tx.n_sym * 48);
	for(int i = 0; i < d_tx.n_sym * 48; i++) {
		symbols[i] = std::complex<double>(sym[i]);
	}

	switch(d_ofdm.encoding) {
	case BPSK_1_2:
	case BPSK_3_4:
		bits = to_vec(demodulate_bpsk(symbols));
		break;

	case QPSK_1_2:
	case QPSK_3_4:
		bits = to_vec(demodulate_qpsk(symbols));
		break;

	case QAM16_1_2:
	case QAM16_3_4:
		bits = to_vec(demodulate_16qam(symbols));
		break;
	case QAM64_2_3:
	case QAM64_3_4:
		bits = to_vec(demodulate_64qam(symbols));
		break;
	}
}

void deinterleave() {

	int n_cbps = d_ofdm.n_cbps;
	int first[n_cbps];
	int second[n_cbps];
	int s = std::max(d_ofdm.n_bpsc / 2, 1);

	for(int j = 0; j < n_cbps; j++) {
		first[j] = s * (j / s) + ((j + int(floor(16.0 * j / n_cbps))) % s);
	}

	for(int i = 0; i < n_cbps; i++) {
		second[i] = 16 * i - (n_cbps - 1) * int(floor(16.0 * i / n_cbps));
	}

	int count = 0;
	for(int i = 0; i < d_tx.n_sym; i++) {
		for(int k = 0; k < n_cbps; k++) {
			if (bits[i * n_cbps + k] > 0)
			{
				d_tmp_inbits[i * n_cbps + second[first[k]]] = 0x01;
			}
			else
			{
				d_tmp_inbits[i * n_cbps + second[first[k]]] = 0x00;
			}
		}
	}

	// Set the appropiate coding rate for
	// the convolutional decoder.
	set_coding_rate();

	if (d_ntraceback == 5)
	{
		count = d_tx.n_sym * n_cbps;
		d_inbits = d_tmp_inbits;
	}
	else
	{
		d_inbits = d_inbits_punctured;
		count = 0;
		for(int i = 0; i < d_tx.n_sym; i++) {
			for(int k = 0; k < n_cbps; k++) {
				while (d_puncture[count % (2 * d_k)] == 0)
				{
					d_inbits[count] = 2;
					count++;
				}

	           // Insert received bits
	           d_inbits[count] = d_tmp_inbits[i * n_cbps + k];
	           count++;

	           while (d_puncture[count % (2 * d_k)] == 0)
				{
					d_inbits[count] = 2;
					count++;
				}
			}
		}
	}

	for (int i = count; i < NeededBits(); i++)
		d_inbits[i] = 0x00;
}

int NeededBits()
{
	int return_value;
	if (d_ntraceback == 5)
		return_value = d_tx.n_encoded_bits / 2;
	else if (d_ntraceback == 9)
	{
		return_value = (d_tx.n_encoded_bits / 3) * 2;
	}
	else // si d_ntraceback == 10
	{
		return_value = (d_tx.n_encoded_bits / 4) * 3;
	}
	return return_value;
}

void decode_conv_viterbi() {

  d_viterbi_chunks_init(state0);
  d_viterbi_chunks_init_sse2(metric0, path0);

  int out_count = 0;
  decoded_count = 0;

  int in_count = 0;
  int neededBits = NeededBits();
  while(decoded_count < neededBits)
        {
          if ((in_count % 4) == 0) //0 or 3
          {
            d_viterbi_butterfly2_sse2(&d_inbits[in_count & 0xfffffffc], metric0, metric1, path0, path1);

            if ((in_count > 0) && (in_count % 16) == 8) // 8 or 11
            {
              unsigned char c;

              d_viterbi_get_output_sse2(metric0, path0, d_ntraceback, &c);

              if (d_init == 0)
              {
                if (out_count >= d_ntraceback)
                {
                	for (int i= 0; i < 8; i++)
                	{
                		out_bytes_viterbi[(out_count - d_ntraceback) * 8 + i] = (c >> (7 - i)) & 0x1;
                		decoded_count++;
                	}
                }
              }
              else
              {
              	out_bytes_viterbi[out_count] = c;
                  for (int i= 0; i < 8; i++)
                  {
                  	out_bytes_viterbi[out_count * 8 + i] = (c >> (7 - i)) & 0x1;
                  	decoded_count++;
                  }
              }
              out_count++;
            }
          }
          in_count++;
        }
}

void set_coding_rate()
{
	switch(d_ofdm.encoding) {
	case BPSK_1_2:
	case QPSK_1_2:
	case QAM16_1_2:
		// Code rate 1/2
		d_ntraceback = 5;
		d_puncture = d_puncture_1_2;
		d_k = 1;
		break;
	case QAM64_2_3:
		// Code rate 2/3
		d_ntraceback = 9;
		d_puncture = d_puncture_2_3;
		d_k = 2;
		break;
	case BPSK_3_4:
	case QPSK_3_4:
	case QAM16_3_4:
	case QAM64_3_4:
		// Code rate 3/4
		d_ntraceback = 10;
		d_puncture = d_puncture_3_4;
		d_k = 3;
		break;
	}
}

void decode_conv() {
	decode_conv_viterbi();
}

void descramble () {
	int state = 0;
	for(int i = 0; i < 7; i++) {
		if(out_bytes_viterbi[i]) {
			state |= 1 << (6 - i);
		}
	}

	int feedback;

	for(int i = 7; i < decoded_count; i++) {
		feedback = ((!!(state & 64))) ^ (!!(state & 8));
		out_bits[i] = feedback ^ (out_bytes_viterbi[i] & 0x1);
		state = ((state << 1) & 0x7e) | feedback;
	}

	for(int i = 0; i < decoded_count; i++) {
		int bit = i % 8;
		int byte = i / 8;
		if(bit == 0) {
			out_bytes[byte] = 0;
		}

		if(out_bits[i]) {
			out_bytes[byte] |= (1 << bit);
		}
	}
}

void print_output() {

	dout << std::endl;
	for(int i = 0; i < decoded_bits.size() / 8; i++) {
		dout << std::setfill('0') << std::setw(2) << std::hex << ((unsigned int)out_bytes[i] & 0xFF) << std::dec << " ";
		if(i % 16 == 15) {
			dout << std::endl;
		}
	}
	dout << std::endl;
	for(int i = 0; i < decoded_bits.size() / 8; i++) {
		if((out_bytes[i] > 31) && (out_bytes[i] < 127)) {
			dout << ((char) out_bytes[i]);
		} else {
			dout << ".";
		}
	}
	dout << std::endl;
}

private:
	gr_complex sym[1000 * 48 * 100];
	vec bits;
	unsigned char* d_inbits;
	unsigned char d_inbits_punctured[1000 * 48];
	unsigned char d_tmp_inbits[1000 * 48];
	char out_bits[40000];
	char out_bytes[40000];
	char out_bytes_viterbi[40000 * 8];
	bvec decoded_bits;

	int d_ntraceback;
	
	// number of input bits in the convolutional encoder.
	int d_k;
	
	const unsigned char * d_puncture;
	const unsigned char d_puncture_1_2[2] = {1, 1};
	const unsigned char d_puncture_2_3[4] = {1, 1, 1, 0};
	const unsigned char d_puncture_3_4[6] = {1, 1, 1, 0, 0, 1};

	bool d_debug;
	bool d_log;
	tx_param d_tx;
	ofdm_param d_ofdm;
	int copied;
	bool d_frame_complete;
};

ofdm_decode_mac::sptr
ofdm_decode_mac::make(bool log, bool debug) {
	return gnuradio::get_initial_sptr(new ofdm_decode_mac_impl(log, debug));
}

