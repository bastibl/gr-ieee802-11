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
#include <ieee802-11/ofdm_preamble.h>

#include "utils.h"
#include <gnuradio/io_signature.h>

using namespace gr::ieee802_11;


class ofdm_preamble_impl : public ofdm_preamble {

public:
ofdm_preamble_impl(bool debug) : tagged_stream_block("ofdm_preamble",
			gr::io_signature::make(1, 1, sizeof(gr_complex)),
			gr::io_signature::make(1, 1, sizeof(gr_complex)),
			"packet_len"),
			d_debug(debug) {
	set_tag_propagation_policy(block::TPP_DONT);
}

~ofdm_preamble_impl(){
}

int calculate_output_stream_length(const gr_vector_int &ninput_items) {
	return ninput_items[0] + 320;
}

int work (int noutput_items, gr_vector_int& ninput_items,
		gr_vector_const_void_star& input_items,
		gr_vector_void_star& output_items) {

	const gr_complex *in = (const gr_complex*)input_items[0];
	gr_complex *out = (gr_complex*)output_items[0];

	std::memcpy(out, PREAMBLE, 320 * sizeof(gr_complex));
	std::memcpy(out + 320, in, ninput_items[0] * sizeof(gr_complex));


	int produced = ninput_items[0] + 320;
	const pmt::pmt_t value = pmt::PMT_T;
	const pmt::pmt_t src = pmt::string_to_symbol(alias());

	add_item_tag(0, nitems_written(0), pmt::mp("tx_sob"), value, src);
	add_item_tag(0, nitems_written(0) + produced - 1, pmt::mp("tx_eob"), value, src);

	return produced;
}

void insert_eob(uint64_t item) {
	dout << "OFDM PAD: insert eob at: " << item << std::endl;

	const pmt::pmt_t eob_key = pmt::string_to_symbol("tx_eob");
	const pmt::pmt_t value = pmt::PMT_T;
	const pmt::pmt_t srcid = pmt::string_to_symbol(this->name());
}

private:
	bool   d_debug;
	static gr_complex PREAMBLE[320];
};

ofdm_preamble::sptr
ofdm_preamble::make(bool debug) {
	return gnuradio::get_initial_sptr(new ofdm_preamble_impl(debug));
}

gr_complex ofdm_preamble_impl::PREAMBLE[320] = {
		gr_complex( 0.2041,  0.2041), gr_complex(-1.1755,  0.0208), gr_complex(-0.1196, -0.6969), gr_complex( 1.2670, -0.1123),
		gr_complex( 0.8165,  0.0000), gr_complex( 1.2670, -0.1123), gr_complex(-0.1196, -0.6969), gr_complex(-1.1755,  0.0208),
		gr_complex( 0.4082,  0.4082), gr_complex( 0.0208, -1.1755), gr_complex(-0.6969, -0.1196), gr_complex(-0.1123,  1.2670),
		gr_complex( 0.0000,  0.8165), gr_complex(-0.1123,  1.2670), gr_complex(-0.6969, -0.1196), gr_complex( 0.0208, -1.1755),
		gr_complex( 0.4082,  0.4082), gr_complex(-1.1755,  0.0208), gr_complex(-0.1196, -0.6969), gr_complex( 1.2670, -0.1123),
		gr_complex( 0.8165,  0.0000), gr_complex( 1.2670, -0.1123), gr_complex(-0.1196, -0.6969), gr_complex(-1.1755,  0.0208),
		gr_complex( 0.4082,  0.4082), gr_complex( 0.0208, -1.1755), gr_complex(-0.6969, -0.1196), gr_complex(-0.1123,  1.2670),
		gr_complex( 0.0000,  0.8165), gr_complex(-0.1123,  1.2670), gr_complex(-0.6969, -0.1196), gr_complex( 0.0208, -1.1755),
		gr_complex( 0.4082,  0.4082), gr_complex(-1.1755,  0.0208), gr_complex(-0.1196, -0.6969), gr_complex( 1.2670, -0.1123),
		gr_complex( 0.8165,  0.0000), gr_complex( 1.2670, -0.1123), gr_complex(-0.1196, -0.6969), gr_complex(-1.1755,  0.0208),
		gr_complex( 0.4082,  0.4082), gr_complex( 0.0208, -1.1755), gr_complex(-0.6969, -0.1196), gr_complex(-0.1123,  1.2670),
		gr_complex( 0.0000,  0.8165), gr_complex(-0.1123,  1.2670), gr_complex(-0.6969, -0.1196), gr_complex( 0.0208, -1.1755),
		gr_complex( 0.4082,  0.4082), gr_complex(-1.1755,  0.0208), gr_complex(-0.1196, -0.6969), gr_complex( 1.2670, -0.1123),
		gr_complex( 0.8165,  0.0000), gr_complex( 1.2670, -0.1123), gr_complex(-0.1196, -0.6969), gr_complex(-1.1755,  0.0208),
		gr_complex( 0.4082,  0.4082), gr_complex( 0.0208, -1.1755), gr_complex(-0.6969, -0.1196), gr_complex(-0.1123,  1.2670),
		gr_complex( 0.0000,  0.8165), gr_complex(-0.1123,  1.2670), gr_complex(-0.6969, -0.1196), gr_complex( 0.0208, -1.1755),
		gr_complex( 0.4082,  0.4082), gr_complex(-1.1755,  0.0208), gr_complex(-0.1196, -0.6969), gr_complex( 1.2670, -0.1123),
		gr_complex( 0.8165,  0.0000), gr_complex( 1.2670, -0.1123), gr_complex(-0.1196, -0.6969), gr_complex(-1.1755,  0.0208),
		gr_complex( 0.4082,  0.4082), gr_complex( 0.0208, -1.1755), gr_complex(-0.6969, -0.1196), gr_complex(-0.1123,  1.2670),
		gr_complex( 0.0000,  0.8165), gr_complex(-0.1123,  1.2670), gr_complex(-0.6969, -0.1196), gr_complex( 0.0208, -1.1755),
		gr_complex( 0.4082,  0.4082), gr_complex(-1.1755,  0.0208), gr_complex(-0.1196, -0.6969), gr_complex( 1.2670, -0.1123),
		gr_complex( 0.8165,  0.0000), gr_complex( 1.2670, -0.1123), gr_complex(-0.1196, -0.6969), gr_complex(-1.1755,  0.0208),
		gr_complex( 0.4082,  0.4082), gr_complex( 0.0208, -1.1755), gr_complex(-0.6969, -0.1196), gr_complex(-0.1123,  1.2670),
		gr_complex( 0.0000,  0.8165), gr_complex(-0.1123,  1.2670), gr_complex(-0.6969, -0.1196), gr_complex( 0.0208, -1.1755),
		gr_complex( 0.4082,  0.4082), gr_complex(-1.1755,  0.0208), gr_complex(-0.1196, -0.6969), gr_complex( 1.2670, -0.1123),
		gr_complex( 0.8165,  0.0000), gr_complex( 1.2670, -0.1123), gr_complex(-0.1196, -0.6969), gr_complex(-1.1755,  0.0208),
		gr_complex( 0.4082,  0.4082), gr_complex( 0.0208, -1.1755), gr_complex(-0.6969, -0.1196), gr_complex(-0.1123,  1.2670),
		gr_complex( 0.0000,  0.8165), gr_complex(-0.1123,  1.2670), gr_complex(-0.6969, -0.1196), gr_complex( 0.0208, -1.1755),
		gr_complex( 0.4082,  0.4082), gr_complex(-1.1755,  0.0208), gr_complex(-0.1196, -0.6969), gr_complex( 1.2670, -0.1123),
		gr_complex( 0.8165,  0.0000), gr_complex( 1.2670, -0.1123), gr_complex(-0.1196, -0.6969), gr_complex(-1.1755,  0.0208),
		gr_complex( 0.4082,  0.4082), gr_complex( 0.0208, -1.1755), gr_complex(-0.6969, -0.1196), gr_complex(-0.1123,  1.2670),
		gr_complex( 0.0000,  0.8165), gr_complex(-0.1123,  1.2670), gr_complex(-0.6969, -0.1196), gr_complex( 0.0208, -1.1755),
		gr_complex( 0.4082,  0.4082), gr_complex(-1.1755,  0.0208), gr_complex(-0.1196, -0.6969), gr_complex( 1.2670, -0.1123),
		gr_complex( 0.8165,  0.0000), gr_complex( 1.2670, -0.1123), gr_complex(-0.1196, -0.6969), gr_complex(-1.1755,  0.0208),
		gr_complex( 0.4082,  0.4082), gr_complex( 0.0208, -1.1755), gr_complex(-0.6969, -0.1196), gr_complex(-0.1123,  1.2670),
		gr_complex( 0.0000,  0.8165), gr_complex(-0.1123,  1.2670), gr_complex(-0.6969, -0.1196), gr_complex( 0.0208, -1.1755),
		gr_complex( 0.4082,  0.4082), gr_complex(-1.1755,  0.0208), gr_complex(-0.1196, -0.6969), gr_complex( 1.2670, -0.1123),
		gr_complex( 0.8165,  0.0000), gr_complex( 1.2670, -0.1123), gr_complex(-0.1196, -0.6969), gr_complex(-1.1755,  0.0208),
		gr_complex( 0.4082,  0.4082), gr_complex( 0.0208, -1.1755), gr_complex(-0.6969, -0.1196), gr_complex(-0.1123,  1.2670),
		gr_complex( 0.0000,  0.8165), gr_complex(-0.1123,  1.2670), gr_complex(-0.6969, -0.1196), gr_complex( 0.0208, -1.1755),
		gr_complex(-0.4893,  0.2041), gr_complex( 0.1090, -0.8662), gr_complex( 0.8140, -0.9396), gr_complex(-0.8155, -1.0218),
		gr_complex(-0.0249, -0.4773), gr_complex( 0.6663,  0.6571), gr_complex(-1.1300,  0.1820), gr_complex(-1.0818,  0.1470),
		gr_complex(-0.3110,  1.3392), gr_complex(-0.5011,  0.1935), gr_complex(-0.5353, -0.7214), gr_complex( 0.6173, -0.1253),
		gr_complex( 0.7297, -0.8197), gr_complex(-1.1650, -0.5789), gr_complex(-0.5077, -0.3488), gr_complex( 0.3277, -0.8728),
		gr_complex( 0.5547,  0.5547), gr_complex( 1.0583,  0.0363), gr_complex(-0.1995, -1.4259), gr_complex( 0.5207,  0.1326),
		gr_complex( 0.2172,  0.5195), gr_complex(-1.2142,  0.4205), gr_complex( 0.0088,  1.0207), gr_complex( 0.4734, -0.0362),
		gr_complex( 0.8657,  0.2298), gr_complex(-0.3401,  0.9423), gr_complex(-1.0218,  0.4897), gr_complex( 0.5309,  0.7784),
		gr_complex( 0.1874, -0.2475), gr_complex( 0.8594, -0.7348), gr_complex( 0.3528,  0.9865), gr_complex(-0.0455,  1.0679),
		gr_complex( 1.3868,  0.0000), gr_complex(-0.0455, -1.0679), gr_complex( 0.3528, -0.9865), gr_complex( 0.8594,  0.7348),
		gr_complex( 0.1874,  0.2475), gr_complex( 0.5309, -0.7784), gr_complex(-1.0218, -0.4897), gr_complex(-0.3401, -0.9423),
		gr_complex( 0.8657, -0.2298), gr_complex( 0.4734,  0.0362), gr_complex( 0.0088, -1.0207), gr_complex(-1.2142, -0.4205),
		gr_complex( 0.2172, -0.5195), gr_complex( 0.5207, -0.1326), gr_complex(-0.1995,  1.4259), gr_complex( 1.0583, -0.0363),
		gr_complex( 0.5547, -0.5547), gr_complex( 0.3277,  0.8728), gr_complex(-0.5077,  0.3488), gr_complex(-1.1650,  0.5789),
		gr_complex( 0.7297,  0.8197), gr_complex( 0.6173,  0.1253), gr_complex(-0.5353,  0.7214), gr_complex(-0.5011, -0.1935),
		gr_complex(-0.3110, -1.3392), gr_complex(-1.0818, -0.1470), gr_complex(-1.1300, -0.1820), gr_complex( 0.6663, -0.6571),
		gr_complex(-0.0249,  0.4773), gr_complex(-0.8155,  1.0218), gr_complex( 0.8140,  0.9396), gr_complex( 0.1090,  0.8662),
		gr_complex(-1.3868,  0.0000), gr_complex( 0.1090, -0.8662), gr_complex( 0.8140, -0.9396), gr_complex(-0.8155, -1.0218),
		gr_complex(-0.0249, -0.4773), gr_complex( 0.6663,  0.6571), gr_complex(-1.1300,  0.1820), gr_complex(-1.0818,  0.1470),
		gr_complex(-0.3110,  1.3392), gr_complex(-0.5011,  0.1935), gr_complex(-0.5353, -0.7214), gr_complex( 0.6173, -0.1253),
		gr_complex( 0.7297, -0.8197), gr_complex(-1.1650, -0.5789), gr_complex(-0.5077, -0.3488), gr_complex( 0.3277, -0.8728),
		gr_complex( 0.5547,  0.5547), gr_complex( 1.0583,  0.0363), gr_complex(-0.1995, -1.4259), gr_complex( 0.5207,  0.1326),
		gr_complex( 0.2172,  0.5195), gr_complex(-1.2142,  0.4205), gr_complex( 0.0088,  1.0207), gr_complex( 0.4734, -0.0362),
		gr_complex( 0.8657,  0.2298), gr_complex(-0.3401,  0.9423), gr_complex(-1.0218,  0.4897), gr_complex( 0.5309,  0.7784),
		gr_complex( 0.1874, -0.2475), gr_complex( 0.8594, -0.7348), gr_complex( 0.3528,  0.9865), gr_complex(-0.0455,  1.0679),
		gr_complex( 1.3868,  0.0000), gr_complex(-0.0455, -1.0679), gr_complex( 0.3528, -0.9865), gr_complex( 0.8594,  0.7348),
		gr_complex( 0.1874,  0.2475), gr_complex( 0.5309, -0.7784), gr_complex(-1.0218, -0.4897), gr_complex(-0.3401, -0.9423),
		gr_complex( 0.8657, -0.2298), gr_complex( 0.4734,  0.0362), gr_complex( 0.0088, -1.0207), gr_complex(-1.2142, -0.4205),
		gr_complex( 0.2172, -0.5195), gr_complex( 0.5207, -0.1326), gr_complex(-0.1995,  1.4259), gr_complex( 1.0583, -0.0363),
		gr_complex( 0.5547, -0.5547), gr_complex( 0.3277,  0.8728), gr_complex(-0.5077,  0.3488), gr_complex(-1.1650,  0.5789),
		gr_complex( 0.7297,  0.8197), gr_complex( 0.6173,  0.1253), gr_complex(-0.5353,  0.7214), gr_complex(-0.5011, -0.1935),
		gr_complex(-0.3110, -1.3392), gr_complex(-1.0818, -0.1470), gr_complex(-1.1300, -0.1820), gr_complex( 0.6663, -0.6571),
		gr_complex(-0.0249,  0.4773), gr_complex(-0.8155,  1.0218), gr_complex( 0.8140,  0.9396), gr_complex( 0.1090,  0.8662),
		gr_complex(-1.3868,  0.0000), gr_complex( 0.1090, -0.8662), gr_complex( 0.8140, -0.9396), gr_complex(-0.8155, -1.0218),
		gr_complex(-0.0249, -0.4773), gr_complex( 0.6663,  0.6571), gr_complex(-1.1300,  0.1820), gr_complex(-1.0818,  0.1470),
		gr_complex(-0.3110,  1.3392), gr_complex(-0.5011,  0.1935), gr_complex(-0.5353, -0.7214), gr_complex( 0.6173, -0.1253),
		gr_complex( 0.7297, -0.8197), gr_complex(-1.1650, -0.5789), gr_complex(-0.5077, -0.3488), gr_complex( 0.3277, -0.8728),
		gr_complex( 0.5547,  0.5547), gr_complex( 1.0583,  0.0363), gr_complex(-0.1995, -1.4259), gr_complex( 0.5207,  0.1326),
		gr_complex( 0.2172,  0.5195), gr_complex(-1.2142,  0.4205), gr_complex( 0.0088,  1.0207), gr_complex( 0.4734, -0.0362),
		gr_complex( 0.8657,  0.2298), gr_complex(-0.3401,  0.9423), gr_complex(-1.0218,  0.4897), gr_complex( 0.5309,  0.7784),
		gr_complex( 0.1874, -0.2475), gr_complex( 0.8594, -0.7348), gr_complex( 0.3528,  0.9865), gr_complex(-0.0455,  1.0679)
		};
