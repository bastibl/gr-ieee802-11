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
#include <ieee802-11/ofdm_pilot.h>

#include "utils.h"
#include <gnuradio/io_signature.h>

using namespace gr::ieee802_11;


class ofdm_pilot_impl : public ofdm_pilot {

static const int TONES = 48;
static const int FFT_SIZE = 64;

public:

ofdm_pilot_impl(bool debug) :
		sync_block ("ofdm_pilot",
				gr::io_signature::make(1, 1, TONES * sizeof(gr_complex)),
				gr::io_signature::make(1, 1, FFT_SIZE * sizeof(gr_complex))),
		d_offset(0),
		d_debug(debug) {

}

~ofdm_pilot_impl() {

}

int work (int noutput_items, gr_vector_const_void_star& input_items,
		gr_vector_void_star& output_items) {

	const gr_complex *in = (const gr_complex*)input_items[0];
	gr_complex *out = (gr_complex*)output_items[0];

	int i = 0;
	int count = 0;
	const uint64_t nread = nitems_read(0);
	std::vector<gr::tag_t> tags;

	while (count < noutput_items) {

		get_tags_in_range(tags, 0, nread + count, nread + count + 1, pmt::string_to_symbol("ofdm_start"));

		if(tags.size()) {
			d_offset = 0;
			dout << "OFDM PILOT: start of OFDM frame" << std::endl;
		}

		// guard band
		memset(out, 0, 6 * sizeof(gr_complex));

		// subcarriers -26 to -22
		memcpy(out + 6, in, 5 * sizeof(gr_complex));

		// pilot 1
		out[11] = POLARITY[d_offset];

		// subcarriers -20 to -8
		memcpy(out + 12, in + 5, 13 * sizeof(gr_complex));

		// pilot 2
		out[25] = POLARITY[d_offset];

		// subcarriers -6 to -1
		memcpy(out + 26, in + 18, 6 * sizeof(gr_complex));

		// DC
		out[32] = 0;

		// subcarriers 1 to 6
		memcpy(out + 33, in + 24, 6 * sizeof(gr_complex));

		// pilot 3
		out[39] = POLARITY[d_offset];

		// pubcarriers 8 to 20
		memcpy(out + 40, in + 30, 13 * sizeof(gr_complex));

		// pilot 4
		out[53] = -POLARITY[d_offset];

		// subcarriers 22 to 26
		memcpy(out + 54, in + 43, 5 * sizeof(gr_complex));

		// guard band
		memset(out + 59, 0, 5 * sizeof(gr_complex));


		count++;
		out += FFT_SIZE;
		in += TONES;
		d_offset = (d_offset + 1) % 127;
	}

	dout << "OFDM PILOT: produced items: " << noutput_items << std::endl;

	return noutput_items;
}

private:
	bool d_debug;
	int d_offset;
	static const gr_complex POLARITY[127];
};

const gr_complex ofdm_pilot_impl::POLARITY[127] = {
		 1, 1, 1, 1,-1,-1,-1, 1,-1,-1,-1,-1, 1, 1,-1, 1,
		-1,-1, 1, 1,-1, 1, 1,-1, 1, 1, 1, 1, 1, 1,-1, 1,
		 1, 1,-1, 1, 1,-1,-1, 1, 1, 1,-1, 1,-1,-1,-1, 1,
		-1, 1,-1,-1, 1,-1,-1, 1, 1, 1, 1, 1,-1,-1, 1, 1,
		-1,-1, 1,-1, 1,-1, 1, 1,-1,-1,-1, 1, 1,-1,-1,-1,
		-1, 1,-1,-1, 1,-1, 1, 1, 1, 1,-1, 1,-1, 1,-1, 1,
		-1,-1,-1,-1,-1, 1,-1, 1, 1,-1, 1,-1, 1, 1, 1,-1,
		-1, 1,-1,-1,-1, 1, 1, 1,-1,-1,-1,-1,-1,-1,-1 };

ofdm_pilot::sptr
ofdm_pilot::make(bool debug) {
	return gnuradio::get_initial_sptr(new ofdm_pilot_impl(debug));
}

