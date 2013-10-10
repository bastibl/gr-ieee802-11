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
#include "wave_short_message_impl.h"

#include <gnuradio/io_signature.h>
#include <gnuradio/block_detail.h>
#include <string.h>

using namespace gr::ieee802_11;

wave_short_message_impl::wave_short_message_impl(int version) :
		block("wave_short_message",
				gr::io_signature::make(0, 0, 0),
				gr::io_signature::make(0, 0, 0)),
		d_version(version) {

	message_port_register_out(pmt::mp("out"));

	message_port_register_in(pmt::mp("in"));
	set_msg_handler(pmt::mp("in"), boost::bind(&wave_short_message_impl::make_frame, this, _1));


	hdr0.version    = 0x00;
	hdr0.security   = 0x00;
	hdr0.channel    = 0x04;
	hdr0.rate       = 0x01;
	hdr0.power      = 0x40;
	hdr0.app_class  = 0x00;
	hdr0.acm_length = 0x00;


	hdr2.version    = 0x02;
	hdr2.psid       = 0x55;
	hdr2.wsm_id     = 0x80;
}

void
wave_short_message_impl::make_frame (pmt::pmt_t msg) {

	if(pmt::is_eof_object(msg)) {
		message_port_pub(pmt::mp("out"), pmt::PMT_EOF);
		detail().get()->set_done(true);
		return;
	} else if(pmt::is_symbol(msg)) {
		std::string s = pmt::symbol_to_string(msg);
		msg = pmt::make_blob(s.data(), s.size());
	} else if(!pmt::is_blob(msg)) {
		throw std::invalid_argument("WSM input has to be a blob");
	}

	uint16_t data_len = pmt::blob_length(msg);

	pmt::pmt_t wsm;
	char *buf;

	switch(d_version) {

	case 0:
		hdr0.length = data_len;
		buf = reinterpret_cast<char*>(std::malloc(data_len + sizeof(wave_hdr_v0)));

		std::memcpy(buf, &hdr0, sizeof(wave_hdr_v0));
		std::memcpy(buf + sizeof(wave_hdr_v0), pmt::blob_data(msg), data_len);

		wsm = pmt::make_blob(buf, data_len + sizeof(wave_hdr_v0));
		break;

	case 2:
		hdr2.length = (data_len << 8) + (data_len >> 8);
		buf = reinterpret_cast<char*>(std::malloc(data_len + sizeof(wave_hdr_v2)));

		std::memcpy(buf, &hdr2, sizeof(wave_hdr_v2));
		std::memcpy(buf + sizeof(wave_hdr_v2), pmt::blob_data(msg), data_len);

		wsm = pmt::make_blob(buf, data_len + sizeof(wave_hdr_v2));
		break;

	default:
		throw std::invalid_argument("wrong version of WSM set");
		break;
	}

	std::free(buf);
	message_port_pub(pmt::mp("out"), wsm);
}

void
wave_short_message_impl::set_version(int version) {
	if(!(version == 0 || version == 2)) {
		throw std::invalid_argument("wrong version of WSM set");
	}
	d_version = version;
}

wave_short_message::sptr
wave_short_message::make(int version) {
	return gnuradio::get_initial_sptr(new wave_short_message_impl(version));
}


