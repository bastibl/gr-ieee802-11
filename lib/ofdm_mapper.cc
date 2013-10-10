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
#include <ieee802-11/ofdm_mapper.h>
#include "utils.h"
#include <gnuradio/io_signature.h>

using namespace gr::ieee802_11;


class ofdm_mapper_impl : public ofdm_mapper {
public:

static const int DATA_CARRIERS = 48;

ofdm_mapper_impl(Encoding e, bool debug) :
	block ("ofdm_mapper",
			gr::io_signature::make(0, 0, 0),
			gr::io_signature::make(1, 1, DATA_CARRIERS * sizeof(gr_complex))),
			d_symbols_offset(0),
			d_symbols(NULL),
			d_debug(debug),
			d_ofdm(e) {

	message_port_register_in(pmt::mp("in"));
	set_encoding(e);
}

~ofdm_mapper_impl() {
	free(d_symbols);
}

void print_message(const char *msg, size_t len) {

	dout << std::hex << "OFDM MAPPER input symbols" << std::endl
		<< "===========================" << std::endl;

	for(int i = 0; i < len; i++) {
		dout << std::hex << (int)msg[i] << " ";
	}

	dout << std::dec << std::endl;
}

void map_to_symbols(const char *bits, size_t len) {

	dout << "OFDM MAPPER complex symbols" << std::endl
		<< "===========================" << std::endl;

	for(int i = 0; i < len; i++) {

		// the signal field is bpsk encoded
		d_symbols[i] = (i < 48) ? bpsk_mapping[bits[i]] : mapping[bits[i]];
		dout << d_symbols[i] << " ";
	}
	dout << std::endl;
}

int general_work(int noutput, gr_vector_int& ninput_items,
			gr_vector_const_void_star& input_items,
			gr_vector_void_star& output_items ) {

	gr_complex *out = (gr_complex*)output_items[0];
	dout << "OFDM MAPPER called offset: " << d_symbols_offset <<
		"   length: " << d_symbols_len << std::endl;

	while(!d_symbols_offset) {
		pmt::pmt_t msg(delete_head_blocking(pmt::intern("in")));

                if(pmt::is_eof_object(msg)) {
			dout << "MAPPER: exiting" << std::endl;
                        return -1;
                }

		if(pmt::is_pair(msg)) {
			dout << "OFDM MAPPER: received new message" << std::endl;
			gr::thread::scoped_lock lock(d_mutex);

			int psdu_length = pmt::blob_length(pmt::cdr(msg));
			const char *psdu = static_cast<const char*>(pmt::blob_data(pmt::cdr(msg)));


			// ############ INSERT MAC STUFF
			tx_param tx(d_ofdm, psdu_length);
			//tx.print_out();
			//ofdm.print_out();

			//alloc memory for modulation steps
			char *data_bits        = (char*)calloc(tx.n_data, sizeof(char));
			char *scrambled_data   = (char*)calloc(tx.n_data, sizeof(char));
			char *encoded_data     = (char*)calloc(tx.n_data * 2, sizeof(char));
			char *punctured_data   = (char*)calloc(tx.n_encoded_bits, sizeof(char));
			char *interleaved_data = (char*)calloc(tx.n_encoded_bits, sizeof(char));
			char *symbols          = (char*)calloc((tx.n_encoded_bits / d_ofdm.n_bpsc) + 48, sizeof(char));

			//generate the OFDM data field, adding service field and pad bits
			generate_bits(psdu, data_bits, tx);
			//print_hex_array(data_bits, tx.n_data);

			// first step, scrambling
			scramble(data_bits, scrambled_data, tx, 23);
			//print_hex_array(scrambled_data, tx.n_data);
			// reset tail bits
			reset_tail_bits(scrambled_data, tx);
			//print_hex_array(scrambled_data, tx.n_data);
			// encoding
			convolutional_encoding(scrambled_data, encoded_data, tx);
			//print_hex_array(encoded_data, tx.n_data * 2);
			// puncturing
			puncturing(encoded_data, punctured_data, tx, d_ofdm);
			//std::cout << "punctured" << std::endl;
			// interleaving
			interleave(punctured_data, interleaved_data, tx, d_ofdm);
			//std::cout << "interleaved" << std::endl;

			split_symbols(interleaved_data, symbols + 48, tx, d_ofdm);
			//std::cout << "split" << std::endl;
			generate_signal_field(symbols, tx, d_ofdm);
			//std::cout << "signal" << std::endl;

			d_symbols_len = (tx.n_sym + 1) * 48;
			assert(d_symbols_len % 48 == 0);


			// ############ INSERT MAC STUFF

			d_symbols = static_cast<gr_complex*>(std::malloc(d_symbols_len * sizeof(gr_complex)));


			print_message(static_cast<const char*>(symbols), d_symbols_len);
			map_to_symbols(static_cast<const char*>(symbols), d_symbols_len);



			// add tag to indicate start of odfm frame
			const pmt::pmt_t key = pmt::string_to_symbol("ofdm_start");
			const pmt::pmt_t value = pmt::from_uint64((d_symbols_len / DATA_CARRIERS));
			const pmt::pmt_t srcid = pmt::string_to_symbol(this->name());
			add_item_tag(0, this->nitems_written(0), key, value, srcid);


			free(data_bits);
			free(scrambled_data);
			free(encoded_data);
			free(punctured_data);
			free(interleaved_data);
			free(symbols);

			break;
		}
	}

	int i = 0;
	while(i < noutput) {

		memcpy(out, d_symbols + d_symbols_offset, DATA_CARRIERS * sizeof(gr_complex));

		i++;
		d_symbols_offset += DATA_CARRIERS;
		out += DATA_CARRIERS;

		if(d_symbols_offset == d_symbols_len) {
			d_symbols_offset = 0;
			free(d_symbols);
			break;
		}
	}

	dout << "OFDM MAPPER: output size: " <<  noutput <<
			"   produced items: " << i << std::endl;
	return i;
}

void set_encoding(Encoding encoding) {

	std::cout << "OFDM MAPPER: encoding: " << encoding << std::endl;
	gr::thread::scoped_lock lock(d_mutex);

	d_ofdm = ofdm_param(encoding);

	switch (encoding) {
	case BPSK_1_2:
	case BPSK_3_4:
		mapping = bpsk_mapping;
		break;

	case QPSK_1_2:
	case QPSK_3_4:
		mapping = qpsk_mapping;
		break;

	case QAM16_1_2:
	case QAM16_3_4:
		mapping = qam16_mapping;
		break;

	case QAM64_2_3:
	case QAM64_3_4:
		mapping = qam64_mapping;
		break;

	default:
		throw std::invalid_argument("wrong encoding");
		break;
	}
}

private:
	bool         d_debug;
	gr_complex*  d_symbols;
	int          d_symbols_offset;
	int          d_symbols_len;
	ofdm_param   d_ofdm;
	gr::thread::mutex d_mutex;

	const        gr_complex *mapping;
	static const gr_complex bpsk_mapping[2];
	static const gr_complex qpsk_mapping[4];
	static const gr_complex qam16_mapping[16];
	static const gr_complex qam64_mapping[64];
};

// constellations for different modulations (normalized to avg power 1)
const gr_complex ofdm_mapper_impl::bpsk_mapping[2] = {
		gr_complex(-1.0, 0.0), gr_complex(1.0, 0.0)};
const gr_complex ofdm_mapper_impl::qpsk_mapping[4] = {
		gr_complex(-0.7071, -0.7071), gr_complex(-0.7071, 0.7071),
		gr_complex(+0.7071, -0.7071), gr_complex(+0.7071, 0.7071)};
const gr_complex ofdm_mapper_impl::qam16_mapping[16] = {
		gr_complex(-0.9487, -0.9487), gr_complex(-0.9487, -0.3162),
		gr_complex(-0.9487, 0.9487), gr_complex(-0.9487, 0.3162),
		gr_complex(-0.3162, -0.9487), gr_complex(-0.3162, -0.3162),
		gr_complex(-0.3162, 0.9487), gr_complex(-0.3162, 0.3162),
		gr_complex(0.9487, -0.9487), gr_complex(0.9487, -0.3162),
		gr_complex(0.9487, 0.9487), gr_complex(0.9487, 0.3162),
		gr_complex(0.3162, -0.9487), gr_complex(0.3162, -0.3162),
		gr_complex(0.3162, 0.9487), gr_complex(0.3162, 0.3162)};
const gr_complex ofdm_mapper_impl::qam64_mapping[64] = {
		gr_complex(-1.0801, -1.0801), gr_complex(-1.0801, -0.7715),
		gr_complex(-1.0801, -0.1543), gr_complex(-1.0801, -0.4629),
		gr_complex(-1.0801, 1.0801), gr_complex(-1.0801, 0.7715),
		gr_complex(-1.0801, 0.1543), gr_complex(-1.0801, 0.4629),
		gr_complex(-0.7715, -1.0801), gr_complex(-0.7715, -0.7715),
		gr_complex(-0.7715, -0.1543), gr_complex(-0.7715, -0.4629),
		gr_complex(-0.7715, 1.0801), gr_complex(-0.7715, 0.7715),
		gr_complex(-0.7715, 0.1543), gr_complex(-0.7715, 0.4629),
		gr_complex(-0.1543, -1.0801), gr_complex(-0.1543, -0.7715),
		gr_complex(-0.1543, -0.1543), gr_complex(-0.1543, -0.4629),
		gr_complex(-0.1543, 1.0801), gr_complex(-0.1543, 0.7715),
		gr_complex(-0.1543, 0.1543), gr_complex(-0.1543, 0.4629),
		gr_complex(-0.4629, -1.0801), gr_complex(-0.4629, -0.7715),
		gr_complex(-0.4629, -0.1543), gr_complex(-0.4629, -0.4629),
		gr_complex(-0.4629, 1.0801), gr_complex(-0.4629, 0.7715),
		gr_complex(-0.4629, 0.1543), gr_complex(-0.4629, 0.4629),
		gr_complex(1.0801, -1.0801), gr_complex(1.0801, -0.7715),
		gr_complex(1.0801, -0.1543), gr_complex(1.0801, -0.4629),
		gr_complex(1.0801, 1.0801), gr_complex(1.0801, 0.7715),
		gr_complex(1.0801, 0.1543), gr_complex(1.0801, 0.4629),
		gr_complex(0.7715, -1.0801), gr_complex(0.7715, -0.7715),
		gr_complex(0.7715, -0.1543), gr_complex(0.7715, -0.4629),
		gr_complex(0.7715, 1.0801), gr_complex(0.7715, 0.7715),
		gr_complex(0.7715, 0.1543), gr_complex(0.7715, 0.4629),
		gr_complex(0.1543, -1.0801), gr_complex(0.1543, -0.7715),
		gr_complex(0.1543, -0.1543), gr_complex(0.1543, -0.4629),
		gr_complex(0.1543, 1.0801), gr_complex(0.1543, 0.7715),
		gr_complex(0.1543, 0.1543), gr_complex(0.1543, 0.4629),
		gr_complex(0.4629, -1.0801), gr_complex(0.4629, -0.7715),
		gr_complex(0.4629, -0.1543), gr_complex(0.4629, -0.4629),
		gr_complex(0.4629, 1.0801), gr_complex(0.4629, 0.7715),
		gr_complex(0.4629, 0.1543), gr_complex(0.4629, 0.4629)};


ofdm_mapper::sptr
ofdm_mapper::make(Encoding mcs, bool debug) {
	return gnuradio::get_initial_sptr(new ofdm_mapper_impl(mcs, debug));
}
