/*
 * Copyright (C) 2014 Bastian Bloessl <bloessl@ccs-labs.org>
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
#include <ieee802-11/wifi_edca.h>

#include <gnuradio/io_signature.h>
#include <gnuradio/block_detail.h>

#include "utils.h"

#include <boost/crc.hpp>
#include <iostream>
#include <stdexcept>

using namespace gr::ieee802_11;

class wifi_edca_impl : public wifi_edca {

public:

wifi_edca_impl(std::vector<uint16_t> cw_min, std::vector<uint16_t>aifsn) :
		block("wifi_edca",
			gr::io_signature::make(0, 0, 0),
			gr::io_signature::make(0, 0, 0)),
		m_cw_min(cw_min), m_aifsn(aifsn)
		{

	if(cw_min.size() != aifsn.size()) {
		throw std::invalid_argument("WiFi EDCA: cw_min and aifsn parameters have to have same length");
	}

	if(cw_min.size() == 0) {
		throw std::invalid_argument("WiFi EDCA need cw_min and aifsn");
	}

	message_port_register_out(pmt::mp("mac out"));

	if(cw_min.size() == 1) {
		message_port_register_in(pmt::mp("ac_in"));
		set_msg_handler(pmt::mp("ac_in"), boost::bind(&wifi_edca_impl::ac_in, this, _1, 0));
	} else {
		for(int i = 0; i < cw_min.size(); i++) {
			std::stringstream sstm;
			sstm << "ac in" << i;
			message_port_register_in(pmt::mp(sstm.str()));
			set_msg_handler(pmt::mp(sstm.str()), boost::bind(&wifi_edca_impl::ac_in, this, _1, i));
		}
	}

}

void ac_in (pmt::pmt_t msg, int i) {
	std::cout << "ac " << i << std::endl;

	if(pmt::is_eof_object(msg)) {
		message_port_pub(pmt::mp("mac out"), pmt::PMT_EOF);
		detail().get()->set_done(true);

	} else if(pmt::is_symbol(msg)) {

		std::string  str;
		str = pmt::symbol_to_string(msg);

		pmt::pmt_t dict = pmt::make_dict();
		dict = pmt::dict_add(dict, pmt::mp("cw_min"), pmt::from_long(m_cw_min[i]));
		dict = pmt::dict_add(dict, pmt::mp("aifsn"), pmt::from_long(m_aifsn[i]));

		pmt::pmt_t blob = pmt::make_blob(str.data(), str.length());
		pmt::pmt_t pair = pmt::cons(dict, blob);

		message_port_pub(pmt::mp("mac out"), pair);

	} else if(pmt::is_pair(msg)) {
		pmt::pmt_t dict = pmt::car(msg);
		dict = pmt::dict_add(dict, pmt::mp("cw_min"), pmt::from_long(m_cw_min[i]));
		dict = pmt::dict_add(dict, pmt::mp("aifsn"), pmt::from_long(m_aifsn[i]));
		message_port_pub(pmt::mp("mac out"), pmt::cons(dict, pmt::cdr(msg)));

	} else {
		throw std::invalid_argument("WiFi EDCA expects PDUs or strings");
	}
}

private:
	std::vector<uint16_t>m_cw_min;
	std::vector<uint16_t>m_aifsn;
};

wifi_edca::sptr
wifi_edca::make(std::vector<uint16_t>cw_min, std::vector<uint16_t> aifsn) {
	return gnuradio::get_initial_sptr(new wifi_edca_impl(cw_min, aifsn));
}

