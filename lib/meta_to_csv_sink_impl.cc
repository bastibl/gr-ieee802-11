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
    meta_to_csv_sink::make(const char* file_path, pmt::pmt_t keys)
    {
      return gnuradio::get_initial_sptr
        (new meta_to_csv_sink_impl(file_path, keys));
    }


    /*
     * The private constructor
     */
    meta_to_csv_sink_impl::meta_to_csv_sink_impl(const char* file_path, pmt::pmt_t keys)
      : gr::block("meta_to_csv_sink",
              gr::io_signature::make(0, 0, 0),
              gr::io_signature::make(0, 0, 0)),
        d_keys(keys)
    {
      d_file.open(file_path),

      message_port_register_in(pmt::mp("pdu in"));
      set_msg_handler(pmt::mp("pdu in"), [this](const pmt::pmt_t &pdu) { print_meta(pdu); });
    }

    /*
     * Our virtual destructor.
     */
    meta_to_csv_sink_impl::~meta_to_csv_sink_impl()
    {
      d_file.close();
    }

    void
    meta_to_csv_sink_impl::print_meta(pmt::pmt_t pdu)
    {
      if (!pmt::is_pair(pdu) || !pmt::is_dict(pmt::car(pdu)))
        throw std::runtime_error("received a malformed pdu message");

      /* pmt::pmt_t meta = pmt::car(pdu); */
      /* pmt::pmt_t keys = pmt::dict_keys(meta); */

      /* d_file << pmt::dict_ref(meta, pmt::nth(0, keys), pmt::PMT_NIL); */

      /* for (int i = 0; i < pmt::length(keys); i++) { */
      /*   d_file << "," << pmt::dict_ref(meta, pmt::nth(i, keys), pmt::PMT_NIL); */
      /* } */

      /* d_file << std::endl; */

      pmt::pmt_t meta = pmt::car(pdu);

      d_file << dict_ref(meta, pmt::nth(0, d_keys), pmt::PMT_NIL);

      for (int i = 1; i < pmt::length(d_keys); i++)
        d_file << "," << dict_ref(meta, pmt::nth(i, d_keys), pmt::PMT_NIL);

      d_file << std::endl;
    }

    /* int */
    /* meta_to_csv_sink_impl::work(int noutput_items, */
    /*     gr_vector_const_void_star &input_items, */
    /*     gr_vector_void_star &output_items) */
    /* { */
    /*   /1* const char* in = (const char*) input_items[0]; *1/ */
    /*   int nwritten = 0; */

    /*   do_update(); */

    /*   if (!d_fp) */
    /*     return noutput_items; */

    /*   while (nwritten < noutput_items) { */
    /*     int count = fprintf(d_fp, */

    /*   // Tell runtime system how many output items we produced. */
    /*   return noutput_items; */
    /* } */

    /* bool */
    /* meta_to_csv_sink_impl::stop() */
    /* { */
    /*   do_update(); */
    /*   fflush(d_fp); */
    /*   return true; */
    /* } */

  } /* namespace ieee802_11 */
} /* namespace gr */

