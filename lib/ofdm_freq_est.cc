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
#include <ieee802-11/ofdm_freq_est.h>
#include "utils.h"
#include <gnuradio/io_signature.h>

using namespace gr::ieee802_11;


class ofdm_freq_est_impl : public ofdm_freq_est {

public:
ofdm_freq_est_impl(bool log, bool debug) : block("ofdm_freq_est",
		gr::io_signature::make2(2, 2, sizeof(gr_complex), sizeof(gr_complex)),
		gr::io_signature::make(1, 1, sizeof(gr_complex))),
		d_log(log),
		d_debug(debug),
		d_state(SYNC) {

	set_tag_propagation_policy(block::TPP_DONT);
}

int general_work (int noutput, gr_vector_int& ninput_items,
		gr_vector_const_void_star& input_items,
		gr_vector_void_star& output_items) {

	const gr_complex *in = (const gr_complex*)input_items[0];
	gr_complex *out = (gr_complex*)output_items[0];


	return 0;
}

void forecast (int noutput_items, gr_vector_int &ninput_items_required) {
	ninput_items_required[0] = noutput_items;
}

private:
	enum {SYNC, COPY, RESET} d_state;
	std::vector<gr::tag_t> d_tags;

	const bool d_log;
	const bool d_debug;
};

ofdm_freq_est::sptr
ofdm_freq_est::make(bool log, bool debug) {
	return gnuradio::get_initial_sptr(new ofdm_freq_est_impl(log, debug));
}

