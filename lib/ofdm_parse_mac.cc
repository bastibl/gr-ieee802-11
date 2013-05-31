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
#include <gnuradio/ieee802_11/ofdm_parse_mac.h>
#include "ofdm_parse_mac_impl.h"

#include <gnuradio/io_signature.h>
#include <gnuradio/block_detail.h>
#include <string>

using namespace gr::ieee802_11;

class ofdm_parse_mac_impl : public ofdm_parse_mac {

#define dout d_debug && std::cout

public:

ofdm_parse_mac_impl(bool debug) :
		gr::block("ofdm_parse_mac",
				gr::io_signature::make (0, 0, 0),
				gr::io_signature::make (0, 0, 0)),
		d_debug(debug) {

    message_port_register_out(pmt::mp("out"));

    message_port_register_in(pmt::mp("in"));
    set_msg_handler(pmt::mp("in"), boost::bind(&ofdm_parse_mac_impl::parse, this, _1));
}

~ofdm_parse_mac_impl() {

}

unsigned int update_crc32(unsigned int crc, const char *data, size_t len) {
	int j;
	unsigned int byte, mask;
	static unsigned int table[256];
	/* Set up the table if necesary */
	if (table[1] == 0) {
		for (byte = 0; byte <= 255; byte++) {
			crc = byte;
			for (j = 7; j >= 0; j--) {
				mask = -(crc & 1);
				crc = (crc >> 1) ^ (0xEDB88320 & mask);
			}
			table[byte] = crc;
		}
	}

	/* Calculate the CRC32*/
	size_t i = 0;
	crc = 0xFFFFFFFF;
	for (i = 0; i < len; i++) {
		byte = data[i];    //Get next byte
		crc = (crc >> 8) ^ table[(crc ^ byte) & 0xFF];
	}
	unsigned int crc_reversed;
	crc_reversed = 0x00000000;
	for (j = 31; j >= 0; j--) {
		crc_reversed |= ((crc >> j) & 1) << (31 - j);
	}
	return crc;
}

unsigned int crc32(const char *buf, int len) {
	return update_crc32(0xffffffff, buf, len) ^ 0xffffffff;
}

void parse(pmt::pmt_t msg) {

	if(pmt::is_eof_object(msg)) {
		message_port_pub(pmt::mp("out"), pmt::PMT_EOF);
		detail().get()->set_done(true);
		return;
	}

	assert(pmt::is_blob(msg));

	int data_len = pmt::blob_length(msg);
	mac_header *h = (mac_header*)pmt::blob_data(msg);

	dout << std::endl << "new mac frame  (length " << data_len << ")" << std::endl;
	dout << "=========================================" << std::endl;
	if(data_len < 20) {
		dout << "frame too short to parse (<20)" << std::endl;
		return;
	}
	#define HEX(a) std::hex << std::setfill('0') << std::setw(2) << int(a) << std::dec
	dout << "duration: " << HEX(h->duration >> 8) << " " << HEX(h->duration  & 0xff) << std::endl;
	dout << "frame control: " << HEX(h->frame_control >> 8) << " " << HEX(h->frame_control & 0xff);


        switch((h->frame_control >> 2) & 3) {

		case 0:
			dout << " (MANAGEMENT)" << std::endl;
			parse_management((char*)h, data_len);
			break;
		case 1:
			dout << " (CONTROL)" << std::endl;
			parse_control((char*)h, data_len);
			break;

		case 2:
			dout << " (DATA)" << std::endl;
			parse_data((char*)h, data_len);
			break;

		default:
			dout << " (unknown)" << std::endl;
			break;
	}

	bool crc = check_crc((char*)pmt::blob_data(msg), data_len);
	dout << "crc ";
	dout << (crc ? "correct" : "wrong") << std::endl;


	if(!crc || data_len < 32) {
		return;
	}

	char *frame = (char*)pmt::blob_data(msg);
	frame[data_len - 4] = '\n';

	// DATA
	if((((h->frame_control) >> 2) & 63) == 2) {
		print_ascii(frame + 24, data_len - 24 - 4);
		pmt::pmt_t payload = pmt::make_blob(frame + 24, data_len - 24 - 3);

		message_port_pub(pmt::mp("out"), pmt::cons(pmt::PMT_NIL, payload));
	// QoS Data
	} else if((((h->frame_control) >> 2) & 63) == 34) {
		print_ascii(frame + 26, data_len - 26 - 4);
		pmt::pmt_t payload = pmt::make_blob(frame + 26, data_len - 26 - 3);

		message_port_pub(pmt::mp("out"), pmt::cons(pmt::PMT_NIL, payload));
	}
}

void parse_management(char *buf, int length) {
	mac_header* h = (mac_header*)buf;

	if(length < 24) {
		dout << "too short for a management frame" << std::endl;
		return;
	}

	dout << "Subtype: ";
	switch(((h->frame_control) >> 4) & 0xf) {
		case 0:
			dout << "Association Request";
			break;
		case 1:
			dout << "Association Response";
			break;
		case 2:
			dout << "Reassociation Request";
			break;
		case 3:
			dout << "Reassociation Response";
			break;
		case 4:
			dout << "Probe Request";
			break;
		case 5:
			dout << "Probe Response";
			break;
		case 6:
			dout << "Timing Advertisement";
			break;
		case 7:
			dout << "Reserved";
			break;
		case 8:
			dout << "Beacon" << std::endl;
			if(length < 38) {
				return;
			}
			{
			uint8_t* len = (uint8_t*) (buf + 24 + 13);
			if(length < 38 + *len) {
				return;
			}
			std::string s(buf + 24 + 14, *len);
			dout << "SSID: " << s;
			}
			break;
		case 9:
			dout << "ATIM";
			break;
		case 10:
			dout << "Disassociation";
			break;
		case 11:
			dout << "Authentication";
			break;
		case 12:
			dout << "Deauthentication";
			break;
		case 13:
			dout << "Action";
			break;
		case 14:
			dout << "Action No ACK";
			break;
		case 15:
			dout << "Reserved";
			break;
		default:
			break;
	}
	dout << std::endl;

	dout << "seq nr: " << int(h->seq_control >> 4) << std::endl;
	dout << "mac 1: ";
	print_mac_address(h->addr1, true);
	dout << "mac 2: ";
	print_mac_address(h->addr2, true);
	dout << "mac 3: ";
	print_mac_address(h->addr3, true);

}


void parse_data(char *buf, int length) {
	mac_header* h = (mac_header*)buf;
	if(length < 24) {
		dout << "too short for a data frame" << std::endl;
		return;
	}

	dout << "Subtype: ";
	switch(((h->frame_control) >> 4) & 0xf) {
		case 0:
			dout << "Data";
			break;
		case 1:
			dout << "Data + CF-ACK";
			break;
		case 2:
			dout << "Data + CR-Poll";
			break;
		case 3:
			dout << "Data + CF-ACK + CF-Poll";
			break;
		case 4:
			dout << "Null";
			break;
		case 5:
			dout << "CF-ACK";
			break;
		case 6:
			dout << "CF-Poll";
			break;
		case 7:
			dout << "CF-ACK + CF-Poll";
			break;
		case 8:
			dout << "QoS Data";
			break;
		case 9:
			dout << "QoS Data + CF-ACK";
			break;
		case 10:
			dout << "QoS Data + CF-Poll";
			break;
		case 11:
			dout << "QoS Data + CF-ACK + CF-Poll";
			break;
		case 12:
			dout << "QoS Null";
			break;
		case 13:
			dout << "Reserved";
			break;
		case 14:
			dout << "QoS CF-Poll";
			break;
		case 15:
			dout << "QoS CF-ACK + CF-Poll";
			break;
		default:
			break;
	}
	dout << std::endl;

	dout << "seq nr: " << int(h->seq_control >> 4) << std::endl;
	dout << "mac 1: ";
	print_mac_address(h->addr1, true);
	dout << "mac 2: ";
	print_mac_address(h->addr2, true);
	dout << "mac 3: ";
	print_mac_address(h->addr3, true);
}

void parse_control(char *buf, int length) {
	mac_header* h = (mac_header*)buf;

	dout << "Subtype: ";
	switch(((h->frame_control) >> 4) & 0xf) {
		case 7:
			dout << "Control Wrapper";
			break;
		case 8:
			dout << "Block ACK Requrest";
			break;
		case 9:
			dout << "Block ACK";
			break;
		case 10:
			dout << "PS Poll";
			break;
		case 11:
			dout << "RTS";
			break;
		case 12:
			dout << "CTS";
			break;
		case 13:
			dout << "ACK";
			break;
		case 14:
			dout << "CF-End";
			break;
		case 15:
			dout << "CF-End + CF-ACK";
			break;
		default:
			dout << "Reserved";
			break;
	}
	dout << std::endl;

	dout << "RA: ";
	print_mac_address(h->addr1, true);
	dout << "TA: ";
	print_mac_address(h->addr2, true);

}

void print_mac_address(uint8_t *addr, bool new_line = false) {
	if(!d_debug) {
		return;
	}

	std::cout << std::setfill('0') << std::hex << std::setw(2);

	for(int i = 0; i < 6; i++) {
		std::cout << (int)addr[i];
		if(i != 5) {
			std::cout << ":";
		}
	}

	std::cout << std::dec;

	if(new_line) {
		std::cout << std::endl;
	}
}

void print_ascii(char* buf, int length) {

	for(int i = 0; i < length; i++) {
		if((buf[i] > 31) && (buf[i] < 127)) {
			dout << buf[i];
		} else {
			dout << ".";
		}
	}
	dout << std::endl;
}

bool check_crc(char *data, int len) {
	unsigned int crc = crc32(data, len);
	if(crc == 558161692) {
		return true;
	}
	return false;
}

private:
	bool d_debug;

};

ofdm_parse_mac::sptr
ofdm_parse_mac::make(bool debug) {
	return gnuradio::get_initial_sptr(new ofdm_parse_mac_impl(debug));
}


