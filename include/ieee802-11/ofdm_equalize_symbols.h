/*
 * Copyright (C) 2013, 2015 Bastian Bloessl <bloessl@ccs-labs.org>
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
#ifndef INCLUDED_IEEE802_11_OFDM_EQUALIZE_SYMBOLS_H
#define INCLUDED_IEEE802_11_OFDM_EQUALIZE_SYMBOLS_H

#include <ieee802-11/api.h>
#include <gnuradio/block.h>

enum Equalizer {
	LMS          = 0,
	LINEAR_COMB  = 1,
};

namespace gr {
namespace ieee802_11 {

class IEEE802_11_API ofdm_equalize_symbols : virtual public block
{
public:

	typedef boost::shared_ptr<ofdm_equalize_symbols> sptr;
	static sptr make(Equalizer algo, bool debug = false);
	virtual void set_algorithm(Equalizer algo) = 0;

};

}  // namespace ieee802_11
}  // namespace gr

#endif /* INCLUDED_IEEE802_11_OFDM_EQUALIZE_SYMBOLS_H*/

