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
#ifndef INCLUDED_IEEE802_11_OFDM_DECODE_MAC_H
#define INCLUDED_IEEE802_11_OFDM_DECODE_MAC_H

#include <ieee802-11/api.h>
#include <gnuradio/block.h>

namespace gr {
namespace ieee802_11 {

class IEEE802_11_API ofdm_decode_mac : virtual public block
{
public:

	typedef boost::shared_ptr<ofdm_decode_mac> sptr;
	static sptr make(bool log = false, bool debug = false);

private:
	
	// Puncturing vectors
    static const unsigned char d_puncture_1_2[];
    static const unsigned char d_puncture_2_3[];
    static const unsigned char d_puncture_3_4[];

};

}  // namespace ieee802_11
}  // namespace gr

#endif /* INCLUDED_IEEE802_11_OFDM_DECODE_MAC_H */


