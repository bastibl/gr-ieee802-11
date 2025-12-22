/*
 * Copyright (C) 2025 Nicolas Guillaume <nicol@sguillau.me>
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
#include "append_crc32_impl.h"

#include <gnuradio/block_detail.h>
#include <gnuradio/io_signature.h>

#include <boost/bind/bind.hpp>
#include <boost/crc.hpp>

#include <cstdint>
#include <cstring>
#include <stdexcept>
#include <vector>

using namespace gr::ieee802_11;

append_crc32::sptr append_crc32::make(bool append)
{
    return gnuradio::get_initial_sptr(new append_crc32_impl(append));
}

append_crc32_impl::append_crc32_impl(bool append)
    : block("append_crc32",
            gr::io_signature::make(0, 0, 0),
            gr::io_signature::make(0, 0, 0)),
      d_append(append)
{
    message_port_register_in(pmt::mp("in"));
    set_msg_handler(pmt::mp("in"), boost::bind(&append_crc32_impl::handle, this, boost::placeholders::_1));
    message_port_register_out(pmt::mp("out"));
}

void append_crc32_impl::handle(pmt::pmt_t msg)
{
    if(pmt::is_eof_object(msg))
    {
        detail().get()->set_done(true);
        return;
    }

    if(!pmt::is_pair(msg))
        throw std::invalid_argument("append_crc32 expects PDU pair");

    pmt::pmt_t meta = pmt::car(msg);
    pmt::pmt_t blob = pmt::cdr(msg);

    if(!pmt::is_blob(blob))
        throw std::invalid_argument("append_crc32 expects blob payload");

    if(!d_append)
    {
        message_port_pub(pmt::mp("out"), pmt::cons(meta, blob));
        return;
    }

    size_t len = pmt::blob_length(blob);
    const uint8_t* data = reinterpret_cast<const uint8_t*>(pmt::blob_data(blob));

    boost::crc_32_type result;
    result.process_bytes(data, len);
    uint32_t crc = result.checksum();

    std::vector<uint8_t> out(len + sizeof(uint32_t));
    std::memcpy(out.data(), data, len);
    std::memcpy(out.data() + len, &crc, sizeof(uint32_t));

    pmt::pmt_t out_blob = pmt::make_blob(out.data(), out.size());
    message_port_pub(pmt::mp("out"), pmt::cons(meta, out_blob));
}
