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
#ifndef INCLUDED_IEEE802_11_PACKET_HEADERGENERATOR_IMPL_H
#define INCLUDED_IEEE802_11_PACKET_HEADERGENERATOR_IMPL_H

#include <ieee802-11/packet_headergenerator.h>

namespace gr {
namespace ieee802_11 {

class packet_headergenerator_impl : public packet_headergenerator
{
private:
	gr::digital::packet_header_default::sptr d_formatter;

public:
	packet_headergenerator_impl(
			const digital::packet_header_default::sptr &header_formatter,
			const std::string &len_tag_key
	);
	~packet_headergenerator_impl();

	void remove_length_tags(const std::vector<std::vector<tag_t> > &tags) {};
	int calculate_output_stream_length(const gr_vector_int &ninput_items) { return d_formatter->header_len(); };

	int work(int noutput_items,
			gr_vector_int &ninput_items,
			gr_vector_const_void_star &input_items,
			gr_vector_void_star &output_items);
	};

} // namespace ieee802_11
} // namespace gr

#endif /* INCLUDED_IEEE802_11_PACKET_HEADERGENERATOR_IMPL_H */

