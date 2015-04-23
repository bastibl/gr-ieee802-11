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
#define DIGITAL_API

%include "gnuradio.i"

%include "ieee802_11_swig_doc.i"

%{
#include "gnuradio/digital/packet_header_default.h"

#include "ieee802-11/chunks_to_symbols.h"
#include "ieee802-11/ether_encap.h"
#include "ieee802-11/moving_average_cc.h"
#include "ieee802-11/moving_average_ff.h"
#include "ieee802-11/ofdm_decode_mac.h"
#include "ieee802-11/ofdm_decode_signal.h"
#include "ieee802-11/ofdm_equalize_symbols.h"
#include "ieee802-11/ofdm_mac.h"
#include "ieee802-11/ofdm_mapper.h"
#include "ieee802-11/ofdm_parse_mac.h"
#include "ieee802-11/ofdm_sync_long.h"
#include "ieee802-11/ofdm_sync_short.h"
#include "ieee802-11/wifi_signal_field.h"
%}

%include "gnuradio/digital/packet_header_default.h"

%include "ieee802-11/chunks_to_symbols.h"
%include "ieee802-11/ether_encap.h"
%include "ieee802-11/moving_average_cc.h"
%include "ieee802-11/moving_average_ff.h"
%include "ieee802-11/ofdm_decode_mac.h"
%include "ieee802-11/ofdm_decode_signal.h"
%include "ieee802-11/ofdm_equalize_symbols.h"
%include "ieee802-11/ofdm_mac.h"
%include "ieee802-11/ofdm_mapper.h"
%include "ieee802-11/ofdm_parse_mac.h"
%include "ieee802-11/ofdm_sync_long.h"
%include "ieee802-11/ofdm_sync_short.h"
%include "ieee802-11/wifi_signal_field.h"

GR_SWIG_BLOCK_MAGIC2(ieee802_11, chunks_to_symbols);
GR_SWIG_BLOCK_MAGIC2(ieee802_11, ether_encap);
GR_SWIG_BLOCK_MAGIC2(ieee802_11, moving_average_cc);
GR_SWIG_BLOCK_MAGIC2(ieee802_11, moving_average_ff);
GR_SWIG_BLOCK_MAGIC2(ieee802_11, ofdm_decode_mac);
GR_SWIG_BLOCK_MAGIC2(ieee802_11, ofdm_decode_signal);
GR_SWIG_BLOCK_MAGIC2(ieee802_11, ofdm_equalize_symbols);
GR_SWIG_BLOCK_MAGIC2(ieee802_11, ofdm_mac);
GR_SWIG_BLOCK_MAGIC2(ieee802_11, ofdm_mapper);
GR_SWIG_BLOCK_MAGIC2(ieee802_11, ofdm_parse_mac);
GR_SWIG_BLOCK_MAGIC2(ieee802_11, ofdm_sync_long);
GR_SWIG_BLOCK_MAGIC2(ieee802_11, ofdm_sync_short);

%template(wifi_signal_field_sptr) boost::shared_ptr<gr::ieee802_11::wifi_signal_field>;
%pythoncode %{
wifi_signal_field_sptr.__repr__ = lambda self: "<wifi_signal_field>"
wifi_signal_field = wifi_signal_field .make;
%}
