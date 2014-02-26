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

#ifndef INCLUDED_IEEE802_11_WIFI_SIGNAL_FIELD_IMPL_H
#define INCLUDED_IEEE802_11_WIFI_SIGNAL_FIELD_IMPL_H

#include <ieee802-11/wifi_signal_field.h>
#include "utils.h"

namespace gr {
namespace ieee802_11 {

class wifi_signal_field_impl : public wifi_signal_field
{
public:
	wifi_signal_field_impl();
	~wifi_signal_field_impl();

	bool header_formatter(long packet_len, unsigned char *out,
			const std::vector<tag_t> &tags);

	bool header_parser(const unsigned char *header,
			std::vector<tag_t> &tags);
private:
	int get_bit(int b, int i);
	void generate_signal_field(char *out, tx_param &tx, ofdm_param &ofdm);
};

} // namespace ieee802_11
} // namespace gr

#endif /* INCLUDED_IEEE802_11_WIFI_SIGNAL_FIELD_IMPL_H */

