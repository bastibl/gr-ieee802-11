/* -*- c++ -*- */
/* Copyright 2012 Free Software Foundation, Inc.
 * 
 * This file is part of GNU Radio
 * 
 * GNU Radio is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 * 
 * GNU Radio is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with GNU Radio; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#ifndef INCLUDED_IEEE802_11_PACKET_HEADERGENERATOR_H
#define INCLUDED_IEEE802_11_PACKET_HEADERGENERATOR_H

#include <ieee802-11/api.h>
#include <gnuradio/tagged_stream_block.h>
#include <gnuradio/digital/packet_header_default.h>

namespace gr {
namespace ieee802_11 {

class IEEE802_11_API packet_headergenerator : virtual public tagged_stream_block
{
public:
	typedef boost::shared_ptr<packet_headergenerator> sptr;

	/* \param header_formatter A header formatter object.
	* \param len_tag_key Length tag key. Note that for header generation,
	*                    it is irrelevant which tag names are set in the
	*                    formatter object, only this value is relevant!
	*/
	static sptr make(
		const digital::packet_header_default::sptr &header_formatter,
		const std::string &len_tag_key="packet_len"
	);

	/* \param header_len If this value is given, a packet_header_default
	*                   object is used to generate the header. This is
	*                   the number of bits per header.
	* \param len_tag_key Length tag key.
	*/
	static sptr make(
		long header_len,
		const std::string &len_tag_key = "packet_len");
	};

} // namespace ieee802_11
} // namespace gr

#endif /* INCLUDED_IEEE802_11_PACKET_HEADERGENERATOR_H */

