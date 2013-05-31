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
#include <gnuradio/ieee802_11/ofdm_sync_short.h>
#include <gnuradio/io_signature.h>

#include <iostream>

using namespace gr::ieee802_11;

class ofdm_sync_short_impl : public ofdm_sync_short {

#define dout d_debug && std::cout

public:
ofdm_sync_short_impl(double threshold, unsigned int max_samples,
		unsigned int min_plateau, bool debug) : gr::block("ofdm_sync_short",
			gr::io_signature::make2(2, 2, sizeof(gr_complex), sizeof(float)),
			gr::io_signature::make(1, 1, sizeof(gr_complex))),
			d_debug(debug),
			d_state(SEARCH),
			d_plateau(0),
			MAX_SAMPLES(max_samples),
			MIN_PLATEAU(min_plateau),
			d_threshold(threshold) {

	set_tag_propagation_policy(gr::block::TPP_DONT);
}

~ofdm_sync_short_impl(){
}

int general_work (int noutput_items, gr_vector_int& ninput_items,
		gr_vector_const_void_star& input_items,
		gr_vector_void_star& output_items) {

	const gr_complex *in = (const gr_complex*)input_items[0];
	const float *in2 = (const float*)input_items[1];
	gr_complex *out = (gr_complex*)output_items[0];

	int noutput = noutput_items;
	int ninput = std::min(ninput_items[0], ninput_items[1]);

	// dout << "SHORT noutput : " << noutput << " ninput: " << ninput_items[0] << std::endl;

	switch(d_state) {

	case SEARCH: {
		int i;

		for(i = 0; i < ninput; i++) {
			if(in2[i] > d_threshold) {
				if(d_plateau < MIN_PLATEAU) {
					d_plateau++;

				} else {
					d_state = COPY;
					d_copy_left = MAX_SAMPLES;
					d_plateau = 0;
					insert_tag(nitems_written(0));
					dout << "SHORT Frame!" << std::endl;
					break;
				}
			} else {
				d_plateau = 0;
			}
		}

		consume(0, i);
		consume(1, i);
		return 0;
	}

	case COPY: {

		int to_copy = std::min(d_copy_left, std::min(noutput, ninput));
		std::memcpy(out, in, to_copy * sizeof(gr_complex));

		d_copy_left -= to_copy;

		if(!d_copy_left) {
			d_state = SEARCH;
		}

		dout << "SHORT copied " << to_copy << std::endl;

		consume(0, to_copy);
		consume(1, to_copy);
		return to_copy;
	}
	}

	throw std::runtime_error("sync short: unknown state");
	return 0;
}

void insert_tag(uint64_t item) {
	dout << "SYNC SHORT: insert ofdm_start at: " << item << std::endl;

	const pmt::pmt_t key = pmt::string_to_symbol("ofdm_start");
	const pmt::pmt_t value = pmt::PMT_T;
	const pmt::pmt_t srcid = pmt::string_to_symbol(name());
	add_item_tag(0, item, key, value, srcid);
}

private:
	enum {SEARCH, COPY} d_state;
	int d_copy_left;
	int d_plateau;
	const double d_threshold;
	const bool d_debug;
	const unsigned int MAX_SAMPLES;
	const unsigned int MIN_PLATEAU;
};

ofdm_sync_short::sptr
ofdm_sync_short::make(double threshold, unsigned int max_samples,
		unsigned int min_plateau, bool debug) {
	return gnuradio::get_initial_sptr(new ofdm_sync_short_impl(threshold,
			max_samples, min_plateau, debug));
}
