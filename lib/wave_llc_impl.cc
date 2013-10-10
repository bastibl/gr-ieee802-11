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
#include "wave_llc_impl.h"

#include <gnuradio/io_signature.h>
#include <gnuradio/block_detail.h>
#include <string.h>

using namespace gr::ieee802_11;

wave_llc_impl::wave_llc_impl() :
		block("wave_llc",
				gr::io_signature::make(0, 0, 0),
				gr::io_signature::make(0, 0, 0)) {

	message_port_register_out(pmt::mp("out"));

	message_port_register_in(pmt::mp("in"));
	set_msg_handler(pmt::mp("in"), boost::bind(&wave_llc_impl::make_frame, this, _1));

	d_header[0] = 0xaa;
	d_header[1] = 0xaa;
	d_header[2] = 0x03;
	d_header[3] = 0x00;
	d_header[4] = 0x00;
	d_header[5] = 0x00;
	d_header[6] = 0x88;
	d_header[7] = 0xdc;
}

void
wave_llc_impl::make_frame (pmt::pmt_t msg) {

	if(pmt::is_eof_object(msg)) {
		message_port_pub(pmt::mp("out"), pmt::PMT_EOF);
		detail().get()->set_done(true);
		return;
	} else if(pmt::is_pair(msg)) {
		msg = pmt::cdr(msg);
	} else if(pmt::is_blob(msg)) {

	} else {
		throw std::invalid_argument("wave_llc expects PDU or blob input");
	}

	size_t data_len = pmt::blob_length(msg);

	char *buf = reinterpret_cast<char *>(std::malloc(
			sizeof(d_header) + data_len));

	std::memcpy(buf, d_header, sizeof(d_header));
	std::memcpy(buf + sizeof(d_header), pmt::blob_data(msg), data_len);

	pmt::pmt_t llc_msg = pmt::make_blob(buf, sizeof(d_header) + data_len);
	message_port_pub(pmt::mp("out"), llc_msg);

	std::free(buf);
}

wave_llc::sptr
wave_llc::make() {
	return gnuradio::get_initial_sptr(new wave_llc_impl());
}


