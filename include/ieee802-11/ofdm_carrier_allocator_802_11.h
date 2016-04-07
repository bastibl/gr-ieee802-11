/* -*- c++ -*- */
/* 
 * Copyright 2015 <+YOU OR YOUR COMPANY+>.
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


#ifndef INCLUDED_IEEE802_11_OFDM_CARRIER_ALLOCATOR_802_11_H
#define INCLUDED_IEEE802_11_OFDM_CARRIER_ALLOCATOR_802_11_H

#include <ieee802-11/api.h>
#include <gnuradio/tagged_stream_block.h>

namespace gr {
  namespace ieee802_11 {

    /*!
     * \brief <+description of block+>
     * \ingroup ieee802-11
     *
     */
    class IEEE802_11_API ofdm_carrier_allocator_802_11 : virtual public gr::tagged_stream_block
    {
     public:
      typedef boost::shared_ptr<ofdm_carrier_allocator_802_11> sptr;

      virtual std::string len_tag_key() = 0;
      virtual const int fft_len() = 0;
      virtual std::vector<std::vector<int> > occupied_carriers() = 0;

      /*!
       * \brief Return a shared_ptr to a new instance of ieee802-11::ofdm_carrier_allocator_802_11.
       *
       * To avoid accidental use of raw pointers, ieee802-11::ofdm_carrier_allocator_802_11's
       * constructor is in a private implementation
       * class. ieee802-11::ofdm_carrier_allocator_802_11::make is the public interface for
       * creating new instances.
       */
      static sptr make(
	  int fft_len,
	  const std::vector<std::vector<int> > &occupied_carriers,
	  const std::vector<std::vector<int> > &pilot_carriers,
	  const std::vector<std::vector<gr_complex> > &pilot_symbols,
	  const std::vector<std::vector<gr_complex> > &sync_words,
	  const std::string &len_tag_key = "packet_len",
	  const bool output_is_shifted=true);

	private:
   	bool use_memset;

    };

  } // namespace ieee802-11
} // namespace gr

#endif /* INCLUDED_IEEE802_11_OFDM_CARRIER_ALLOCATOR_802_11_H */

