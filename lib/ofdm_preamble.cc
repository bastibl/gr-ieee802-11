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

static const int SYMBOL_LENGTH = 80;

public:
ofdm_preamble_impl(bool debug) : block("ofdm_preamble",
			gr::io_signature::make(1, 1, sizeof(gr_complex)),
			gr::io_signature::make(1, 1, sizeof(gr_complex))),
			d_debug(debug),
			d_state(IDLE) {

	set_relative_rate(1);
	set_output_multiple(SYMBOL_LENGTH);
	set_tag_propagation_policy(block::TPP_DONT);
}

~ofdm_preamble_impl(){
}

int general_work (int noutput_items, gr_vector_int& ninput_items,
		gr_vector_const_void_star& input_items,
		gr_vector_void_star& output_items) {

	int ninput = ninput_items[0] / SYMBOL_LENGTH;
	//assert(ninput_items[0] % SYMBOL_LENGTH == 0);
	int noutput = noutput_items / SYMBOL_LENGTH;

	const gr_complex *in = (const gr_complex*)input_items[0];
	gr_complex *out = (gr_complex*)output_items[0];

	int written = 0;
	int consumed = 0;

	const uint64_t nread = this->nitems_read(0);
	std::vector<gr::tag_t> tags;
	pmt::pmt_t key = pmt::string_to_symbol("tx_sob");
	pmt::pmt_t srcid = pmt::string_to_symbol(this->name());

	dout << "OFDM PREAMBLE: input size: " << ninput_items[0] << "   output size: "
	     << noutput_items << std::endl;

	while(written < noutput) {

		// debug
		dout << "OFDM PREAMBLE: iteration - consumed: " << consumed <<
				"   written:" << written << std::endl;
		//assert((consumed + 1) * SYMBOL_LENGTH <= ninput_items[0]);

		switch(d_state) {

		case IDLE:

			if(consumed >= ninput) {
				goto out;
			}

			dout << "OFDM PREAMBLE: start of new frame" << std::endl;

			get_tags_in_range(tags, 0, nread + consumed * SYMBOL_LENGTH,
								nread + (consumed + 1) * SYMBOL_LENGTH - 1,
								pmt::string_to_symbol("ofdm_start"));

			// looks like we changed to modulation scheme during the frame
			if(tags.size() != 1) {
				std::cout << "PREAMBLE: frame is longer than expected" << std::endl;
				throw std::runtime_error("PREAMBLE: frame is too long");
				break;
			}
			d_frame_left = pmt::to_uint64(tags[0].value);
			d_preamble_left = 4;
			d_state = PRE;

			// add tag to indicate start of odfm frame
			add_item_tag(0, this->nitems_written(0) + written * SYMBOL_LENGTH, key, pmt::PMT_T, srcid);

			// FALL THROUGH !!!!!

		case PRE:

			dout << "OFDM PREAMBLE: copy preamble" << std::endl;

			memcpy(out, PREAMBLE + (4 - d_preamble_left) * SYMBOL_LENGTH,
					SYMBOL_LENGTH * sizeof(gr_complex));

			out += SYMBOL_LENGTH;
			written++;
			d_preamble_left--;

			if(!d_preamble_left) {
				d_state = FRAME;
			}

			break;

		case FRAME:
			if(consumed >= ninput) {
				goto out;
			}

			dout << "OFDM PREAMBLE: copy frame" << std::endl;

			memcpy(out, in, SYMBOL_LENGTH * sizeof(gr_complex));

			consumed++;
			in += SYMBOL_LENGTH;
			out += SYMBOL_LENGTH;
			written++;
			d_frame_left--;

			if(!d_frame_left) {
				d_state = IDLE;
				insert_eob(nitems_written(0) + written * SYMBOL_LENGTH -1);
				goto out;
			}

			break;

		default:
			assert(false);
			break;
		}
	}

out:

	dout << "PREAMBLE: consumed " << consumed * SYMBOL_LENGTH;
	dout << "    written " << written * SYMBOL_LENGTH << std::endl;
	consume(0, consumed * SYMBOL_LENGTH);
	return written * SYMBOL_LENGTH;
}

void insert_eob(uint64_t item) {
	dout << "OFDM PAD: insert eob at: " << item << std::endl;

	const pmt::pmt_t eob_key = pmt::string_to_symbol("tx_eob");
	const pmt::pmt_t value = pmt::PMT_T;
	const pmt::pmt_t srcid = pmt::string_to_symbol(this->name());
	add_item_tag(0, item, eob_key, value, srcid);
}

void
forecast (int noutput_items, gr_vector_int &ninput_items_required)
{
	if(d_state == PRE) {
		ninput_items_required[0] = 0;
	} else {
		ninput_items_required[0] = noutput_items;
	}

	dout << "forecast noutput_items: " << noutput_items << "  required: " << ninput_items_required[0] << std::endl;
}

private:
	bool   d_debug;
	size_t d_preamble_left;
	size_t d_frame_left;
	enum {IDLE, PRE, FRAME} d_state;

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
