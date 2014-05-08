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

	assert(cw_min.size() == aifsn.size());

	message_port_register_out(pmt::mp("mac out"));

	for(int i = 0; i < cw_min.size(); i++) {
		std::stringstream sstm;
		sstm << "ac in" << i;
		message_port_register_in(pmt::mp(sstm.str()));
		set_msg_handler(pmt::mp(sstm.str()), boost::bind(&wifi_edca_impl::ac_in, this, _1, i));
	}

}

void ac_in (pmt::pmt_t msg, int i) {
	std::cout << "ac " << i << std::endl;
	message_port_pub(pmt::mp("mac out"), msg);
}

private:
	std::vector<uint16_t>m_cw_min;
	std::vector<uint16_t>m_aifsn;
};

wifi_edca::sptr
wifi_edca::make(std::vector<uint16_t>cw_min, std::vector<uint16_t> aifsn) {
	return gnuradio::get_initial_sptr(new wifi_edca_impl(cw_min, aifsn));
}

