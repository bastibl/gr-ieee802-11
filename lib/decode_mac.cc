/*
 * Copyright (C) 2013, 2016 Bastian Bloessl <bloessl@ccs-labs.org>
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
#include <ieee802-11/decode_mac.h>

#include "utils.h"
#include "viterbi_decoder.h"

#include <boost/crc.hpp>
#include <gnuradio/io_signature.h>

using namespace gr::ieee802_11;

#define CANARY_SIZE 64

class decode_mac_impl : public decode_mac {

public:
decode_mac_impl(bool log, bool debug) :
	block("decode_mac",
			gr::io_signature::make(1, 1, 48),
			gr::io_signature::make(0, 0, 0)),
	d_log(log),
	d_debug(debug),
	d_snr(0),
	d_ofdm(BPSK_1_2),
	d_frame(d_ofdm, 0),
	d_frame_complete(true) {

	memset(canary, 0, CANARY_SIZE);

	message_port_register_out(pmt::mp("out"));
}

// poor man's valgrind...
void check_canary(void) {
	for (int i=CANARY_SIZE-1; i>=0; --i) {
		if (canary[i] != 0) {
			// seen in the wild: ERROR: canary overwritten at offset 6
			std::cout << "ERROR: canary overwritten at offset " << i << "\n";
			return;
		}
	}
}

int general_work (int noutput_items, gr_vector_int& ninput_items,
		gr_vector_const_void_star& input_items,
		gr_vector_void_star& output_items) {

	//check_canary();

	const uint8_t *in = (const uint8_t*)input_items[0];

	int i = 0;

	std::vector<gr::tag_t> tags;
	const uint64_t nread = this->nitems_read(0);

	dout << "Decode MAC: input " << ninput_items[0] << std::endl;

	while(i < ninput_items[0]) {

		get_tags_in_range(tags, 0, nread + i, nread + i + 1,
			pmt::string_to_symbol("wifi_start"));

		if(tags.size()) {
			if (d_frame_complete == false) {
				dout << "Warning: starting to receive new frame before old frame was complete" << std::endl;
				dout << "Already copied " << copied << " out of " << d_frame.n_sym << " symbols of last frame" << std::endl;
			}
			d_frame_complete = false;

			pmt::pmt_t tuple = tags[0].value;
			int len_data = pmt::to_uint64(pmt::tuple_ref(tuple, 0));
			int encoding = pmt::to_uint64(pmt::tuple_ref(tuple, 1));
			d_snr = pmt::to_double(pmt::tuple_ref(tuple, 2));

			ofdm_param ofdm = ofdm_param((Encoding)encoding);
			frame_param frame = frame_param(ofdm, len_data);

			// check for maximum frame size
			if(frame.n_sym <= MAX_SYM && frame.psdu_size <= MAX_PSDU_SIZE) {
				d_ofdm = ofdm;
				d_frame = frame;
				copied = 0;
				dout << "Decode MAC: frame start -- len " << len_data
					<< "  symbols " << frame.n_sym << "  encoding "
					<< encoding << std::endl;
			} else {
				dout << "Dropping frame which is too large (symbols or bits)" << std::endl;
			}
		}

		if(copied < d_frame.n_sym) {
			dout << "copy one symbol, copied " << copied << " out of " << d_frame.n_sym << std::endl;
			std::memcpy(d_rx_symbols + (copied * 48), in, 48);
			copied++;

			if(copied == d_frame.n_sym) {
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

	for(int i = 0; i < d_frame.n_sym * 48; i++) {
		for(int k = 0; k < d_ofdm.n_bpsc; k++) {
			d_rx_bits[i*d_ofdm.n_bpsc + k] = !!(d_rx_symbols[i] & (1 << k));
		}
	}

	deinterleave();
	uint8_t *decoded = d_decoder.decode(&d_ofdm, &d_frame, d_deinterleaved_bits);
	descramble(decoded);
	print_output();

	// skip service field
	boost::crc_32_type result;
	result.process_bytes(out_bytes + 2, d_frame.psdu_size);
	if(result.checksum() != 558161692) {
		dout << "checksum wrong -- dropping" << std::endl;
		return;
	}

	mylog(boost::format("encoding: %1% - length: %2% - symbols: %3%")
			% d_ofdm.encoding % d_frame.psdu_size % d_frame.n_sym);

	// create PDU
	pmt::pmt_t blob = pmt::make_blob(out_bytes + 2, d_frame.psdu_size - 4);
	pmt::pmt_t enc = pmt::from_uint64(d_ofdm.encoding);
	pmt::pmt_t dict = pmt::make_dict();
	dict = pmt::dict_add(dict, pmt::mp("encoding"), enc);
	dict = pmt::dict_add(dict, pmt::mp("snr"), pmt::from_double(d_snr));
	message_port_pub(pmt::mp("out"), pmt::cons(dict, blob));
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
	for(int i = 0; i < d_frame.n_sym; i++) {
		for(int k = 0; k < n_cbps; k++) {
			d_deinterleaved_bits[i * n_cbps + second[first[k]]] = d_rx_bits[i * n_cbps + k];
		}
	}
}


void descramble (uint8_t *decoded_bits) {

	int state = 0;
	std::memset(out_bytes, 0, d_frame.psdu_size+2);

	for(int i = 0; i < 7; i++) {
		if(decoded_bits[i]) {
			state |= 1 << (6 - i);
		}
	}
	out_bytes[0] = state;

	int feedback;
	int bit;

	for(int i = 7; i < d_frame.psdu_size*8+16; i++) {
		feedback = ((!!(state & 64))) ^ (!!(state & 8));
		bit = feedback ^ (decoded_bits[i] & 0x1);
		out_bytes[i/8] |= bit << (i%8);
		state = ((state << 1) & 0x7e) | feedback;
	}
}

void print_output() {

	dout << std::endl;
	dout << "psdu size" << d_frame.psdu_size << std::endl;
	for(int i = 2; i < d_frame.psdu_size+2; i++) {
		dout << std::setfill('0') << std::setw(2) << std::hex << ((unsigned int)out_bytes[i] & 0xFF) << std::dec << " ";
		if(i % 16 == 15) {
			dout << std::endl;
		}
	}
	dout << std::endl;
	for(int i = 2; i < d_frame.psdu_size+2; i++) {
		if((out_bytes[i] > 31) && (out_bytes[i] < 127)) {
			dout << ((char) out_bytes[i]);
		} else {
			dout << ".";
		}
	}
	dout << std::endl;
}

private:
	bool d_debug;
	bool d_log;

	frame_param d_frame;
	ofdm_param d_ofdm;
	double d_snr;
	viterbi_decoder d_decoder;

	int copied;
	bool d_frame_complete;

	uint8_t d_rx_symbols[48 * MAX_SYM];
	uint8_t d_rx_bits[MAX_ENCODED_BITS];
	uint8_t d_deinterleaved_bits[MAX_ENCODED_BITS];
	uint8_t out_bytes[MAX_PSDU_SIZE + 2]; // 2 for signal field

	// DO NOT ADD any member below this point, there is a stack corruption 
	// going on. This is a temporary workaround, until the root cause is found:
	// https://en.wikipedia.org/wiki/Stack_buffer_overflow#Stack_canaries
	uint8_t canary[CANARY_SIZE];
};

decode_mac::sptr
decode_mac::make(bool log, bool debug) {
	return gnuradio::get_initial_sptr(new decode_mac_impl(log, debug));
}
