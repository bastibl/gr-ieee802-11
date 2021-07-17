/*
 * Copyright (C) 2013, 2016 Bastian Bloessl <bloessl@ccs-labs.org>
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

#ifndef INCLUDED_IEEE802_11_SIGNAL_FIELD_H
#define INCLUDED_IEEE802_11_SIGNAL_FIELD_H

#include <gnuradio/digital/packet_header_default.h>
#include <ieee802_11/api.h>

namespace gr {
namespace ieee802_11 {

class IEEE802_11_API signal_field : virtual public digital::packet_header_default
{
public:
    typedef boost::shared_ptr<signal_field> sptr;
    static sptr make();

protected:
    signal_field();
};

} // namespace ieee802_11
} // namespace gr

#endif /* INCLUDED_IEEE802_11_SIGNAL_FIELD_H */
