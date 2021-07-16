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

#ifndef INCLUDED_IEEE802_11_META_TO_CSV_SINK_IMPL_H
#define INCLUDED_IEEE802_11_META_TO_CSV_SINK_IMPL_H

#include <ieee802_11/meta_to_csv_sink.h>

namespace gr {
  namespace ieee802_11 {

    class meta_to_csv_sink_impl : public meta_to_csv_sink
    {
     private:
      std::ofstream d_file;
      pmt::pmt_t d_keys;

      void print_meta(pmt::pmt_t pdu);

     public:
      meta_to_csv_sink_impl(const char* file_path, pmt::pmt_t keys);
      ~meta_to_csv_sink_impl();

      /* int work(int noutput_items, */
      /*          gr_vector_const_void_star &input_items, */
      /*          gr_vector_void_star &output_items) override; */
    };

  } // namespace ieee802_11
} // namespace gr

#endif /* INCLUDED_IEEE802_11_META_TO_CSV_SINK_IMPL_H */

