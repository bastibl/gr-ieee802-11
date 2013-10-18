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
#include <ieee802-11/ofdm_mac.h>

#include <gnuradio/io_signature.h>
#include <gnuradio/block_detail.h>

#include "utils.h"

#if defined(__APPLE__)
#include <architecture/byte_order.h>
#define htole16(x) OSSwapHostToLittleInt16(x)
#else
#include <endian.h>
#endif

#include <boost/crc.hpp>
#include <iostream>
#include <stdexcept>

using namespace gr::ieee802_11;

class ofdm_mac_impl : public ofdm_mac {

public:

ofdm_mac_impl() :
		block("ofdm_mac",
			gr::io_signature::make(0, 0, 0),
			gr::io_signature::make(0, 0, 0)),
		d_seq_nr(0) {

	message_port_register_out(pmt::mp("phy out"));
	message_port_register_out(pmt::mp("app out"));

	message_port_register_in(pmt::mp("app in"));
	set_msg_handler(pmt::mp("app in"), boost::bind(&ofdm_mac_impl::app_in, this, _1));

	message_port_register_in(pmt::mp("phy in"));
	set_msg_handler(pmt::mp("phy in"), boost::bind(&ofdm_mac_impl::phy_in, this, _1));
}

void phy_in (pmt::pmt_t msg) {
	message_port_pub(pmt::mp("app out"), msg);
}

void app_in (pmt::pmt_t msg) {

	size_t       msg_len;
	const char   *msdu;

	if(pmt::is_eof_object(msg)) {
		message_port_pub(pmt::mp("phy out"), pmt::PMT_EOF);
		detail().get()->set_done(true);
		return;

	} else if(pmt::is_symbol(msg)) {

		std::string  str;
		str = pmt::symbol_to_string(msg);
		msg_len = str.length();
		msdu = str.data();

	} else if(pmt::is_pair(msg)) {

		msg_len = pmt::blob_length(pmt::cdr(msg));
		msdu = reinterpret_cast<const char *>(pmt::blob_data(pmt::cdr(msg)));

	} else {
		throw std::invalid_argument("OFDM MAC expects PDUs or strings");
                return;
	}

	// make MAC frame
	int    psdu_length;
	char   *psdu;
	generate_mac_data_frame(msdu, msg_len, &psdu, &psdu_length);

	// dict
	pmt::pmt_t dict = pmt::make_dict();
	dict = pmt::dict_add(dict, pmt::mp("crc_included"), pmt::PMT_T);

	// blob
	pmt::pmt_t mac = pmt::make_blob(psdu, psdu_length);

	// pdu
	message_port_pub(pmt::mp("phy out"), pmt::cons(dict, mac));

	free(psdu);
}

void generate_mac_data_frame(const char *msdu, int msdu_size, char **psdu, int *psdu_size) {

	// mac header
	mac_header header;
	header.frame_control = 0x0008;
	header.duration = 0x002e;

	header.addr1[0] = 0x30;
	header.addr1[1] = 0x14;
	header.addr1[2] = 0x4a;
	header.addr1[3] = 0xe6;
	header.addr1[4] = 0x46;
	header.addr1[5] = 0xe4;

	header.addr2[0] = 0x12;
	header.addr2[1] = 0x34;
	header.addr2[2] = 0x56;
	header.addr2[3] = 0x78;
	header.addr2[4] = 0x90;
	header.addr2[5] = 0xab;

	header.addr3[0] = 0x42;
	header.addr3[1] = 0x42;
	header.addr3[2] = 0x42;
	header.addr3[3] = 0x42;
	header.addr3[4] = 0x42;
	header.addr3[5] = 0x42;

	header.seq_nr = 0;
	for (int i = 0; i < 12; i++) {
		if(d_seq_nr & (1 << i)) {
			header.seq_nr |=  (1 << (i + 4));
		}
	}
	header.seq_nr = htole16(header.seq_nr);
	d_seq_nr++;

	//header size is 24, plus 4 for FCS means 28 bytes
	*psdu_size = 28 + msdu_size;
	*psdu = (char *) calloc(*psdu_size, sizeof(char));

	//copy mac header into psdu
	std::memcpy(*psdu, &header, 24);
	//copy msdu into psdu
	memcpy(*psdu + 24, msdu, msdu_size);
	//compute and store fcs
	boost::crc_32_type result;
	result.process_bytes(*psdu, msdu_size + 24);

	unsigned int fcs = result.checksum();
	memcpy(*psdu + msdu_size + 24, &fcs, sizeof(unsigned int));
}

private:
	uint16_t d_seq_nr;
	char d_mac1[6];
	char d_mac2[6];
	char d_mac3[6];
};

ofdm_mac::sptr
ofdm_mac::make() {
	return gnuradio::get_initial_sptr(new ofdm_mac_impl());
}

