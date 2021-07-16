/* -*- c++ -*- */
/*
 * Copyright 2021 Anton Ottosson.
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this software; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gnuradio/io_signature.h>
#include "meta_to_csv_sink_impl.h"

namespace gr {
  namespace ieee802_11 {

    meta_to_csv_sink::sptr
    meta_to_csv_sink::make(const char* file_path, std::vector<std::string> keys, std::string delimiter)
    {
      return gnuradio::get_initial_sptr
        (new meta_to_csv_sink_impl(file_path, keys, delimiter));
    }

    meta_to_csv_sink_impl::meta_to_csv_sink_impl(const char* file_path, std::vector<std::string> keys, std::string delimiter)
      : gr::block("meta_to_csv_sink",
              gr::io_signature::make(0, 0, 0),
              gr::io_signature::make(0, 0, 0)),
        d_delimiter(delimiter)
    {
      d_file.open(file_path);

      d_keys = pmt::make_vector(keys.size(), pmt::PMT_NIL);

      for (int i = 0; i < keys.size(); i++)
        pmt::vector_set(d_keys, i, pmt::mp(keys[i]));

      message_port_register_in(pmt::mp("pdu in"));
      set_msg_handler(pmt::mp("pdu in"), [this](const pmt::pmt_t &pdu) { print_meta(pdu); });
    }

    meta_to_csv_sink_impl::~meta_to_csv_sink_impl()
    {
      d_file.close();
    }

    void
    meta_to_csv_sink_impl::print_meta(pmt::pmt_t pdu)
    {
      if (!pmt::is_pair(pdu) || !pmt::is_dict(pmt::car(pdu)))
        throw std::runtime_error("received a malformed pdu message");

      pmt::pmt_t meta = pmt::car(pdu);

      d_file << dict_ref(meta, pmt::vector_ref(d_keys, 0), pmt::PMT_NIL);

      for (int i = 1; i < pmt::length(d_keys); i++)
        d_file << d_delimiter << dict_ref(meta, pmt::vector_ref(d_keys, i), pmt::PMT_NIL);

      d_file << std::endl;
    }

  } /* namespace ieee802_11 */
} /* namespace gr */

