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
#define IEEE802_11_API

%include "gnuradio.i"

%include "ieee802_11_swig_doc.i"

%{
#include "ieee802-11/ether_encap.h"
#include "ieee802-11/ofdm_decode_mac.h"
#include "ieee802-11/ofdm_decode_signal.h"
#include "ieee802-11/ofdm_equalize_symbols.h"
#include "ieee802-11/ofdm_parse_mac.h"
#include "ieee802-11/ofdm_sync_long.h"
#include "ieee802-11/ofdm_sync_short.h"
%}

%include "ieee802-11/ether_encap.h"
%include "ieee802-11/ofdm_decode_mac.h"
%include "ieee802-11/ofdm_decode_signal.h"
%include "ieee802-11/ofdm_equalize_symbols.h"
%include "ieee802-11/ofdm_parse_mac.h"
%include "ieee802-11/ofdm_sync_long.h"
%include "ieee802-11/ofdm_sync_short.h"

GR_SWIG_BLOCK_MAGIC2(ieee802_11, ether_encap);
GR_SWIG_BLOCK_MAGIC2(ieee802_11, ofdm_decode_mac);
GR_SWIG_BLOCK_MAGIC2(ieee802_11, ofdm_decode_signal);
GR_SWIG_BLOCK_MAGIC2(ieee802_11, ofdm_equalize_symbols);
GR_SWIG_BLOCK_MAGIC2(ieee802_11, ofdm_parse_mac);
GR_SWIG_BLOCK_MAGIC2(ieee802_11, ofdm_sync_long);
GR_SWIG_BLOCK_MAGIC2(ieee802_11, ofdm_sync_short);

