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
#ifndef INCLUDED_IEEE802_11_WIFI_EDCA_H
#define INCLUDED_IEEE802_11_WIFI_EDCA_H

#include <ieee802-11/api.h>
#include <gnuradio/block.h>

namespace gr {
namespace ieee802_11 {

class IEEE802_11_API wifi_edca : virtual public block
{
public:

	typedef boost::shared_ptr<wifi_edca> sptr;
	static sptr make(std::vector<uint16_t>cw_min, std::vector<uint16_t>aifsn);
};

}  // namespace ieee802_11
}  // namespace gr

#endif /* INCLUDED_IEEE802_11_WIFI_EDCA_H */
