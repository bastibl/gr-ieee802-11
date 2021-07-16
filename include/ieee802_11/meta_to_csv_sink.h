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

#ifndef INCLUDED_IEEE802_11_META_TO_CSV_SINK_H
#define INCLUDED_IEEE802_11_META_TO_CSV_SINK_H

#include <ieee802_11/api.h>
#include <gnuradio/block.h>

namespace gr {
  namespace ieee802_11 {

    /*!
     * \brief <+description of block+>
     * \ingroup ieee802_11
     *
     */
    class IEEE802_11_API meta_to_csv_sink : virtual public gr::block
    {
     public:
      typedef boost::shared_ptr<meta_to_csv_sink> sptr;

      /*!
       * \brief Return a shared_ptr to a new instance of ieee802_11::meta_to_csv_sink.
       *
       * To avoid accidental use of raw pointers, ieee802_11::meta_to_csv_sink's
       * constructor is in a private implementation
       * class. ieee802_11::meta_to_csv_sink::make is the public interface for
       * creating new instances.
       */
      static sptr make(const char* file_path, std::vector<std::string> keys, std::string delimiter);
    };

  } // namespace ieee802_11
} // namespace gr

#endif /* INCLUDED_IEEE802_11_META_TO_CSV_SINK_H */

