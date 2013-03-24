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
#define GR_IEEE802_11_API

########################################################################
# standard includes
########################################################################
%include <gnuradio.i>
%include "ieee802_11_factory.i"

########################################################################
# block includes
########################################################################
%include <ieee802_11_ofdm_decode_mac.i>
%include <ieee802_11_ofdm_decode_signal.i>
%include <ieee802_11_ofdm_equalize_symbols.i>
%include <ieee802_11_ofdm_parse_mac.i>
%include <ieee802_11_ofdm_sync_long.i>
%include <ieee802_11_ofdm_sync_short.i>
