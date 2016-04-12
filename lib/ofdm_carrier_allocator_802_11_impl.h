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

#ifndef INCLUDED_IEEE802_11_OFDM_CARRIER_ALLOCATOR_802_11_IMPL_H
#define INCLUDED_IEEE802_11_OFDM_CARRIER_ALLOCATOR_802_11_IMPL_H

#include <ieee802-11/ofdm_carrier_allocator_802_11.h>

namespace gr {
  namespace ieee802_11 {

    class ofdm_carrier_allocator_802_11_impl : public ofdm_carrier_allocator_802_11
    {
     private:
      //! FFT length
      const int d_fft_len;
      //! Which carriers/symbols carry data
      std::vector<std::vector<int> > d_occupied_carriers;
      //! Which carriers/symbols carry pilots symbols
      std::vector<std::vector<int> > d_pilot_carriers;
      //! Value of said pilot symbols
      const std::vector<std::vector<gr_complex> > d_pilot_symbols;
      //! Synch words
      const std::vector<std::vector<gr_complex> > d_sync_words;
      int d_symbols_per_set;
      const bool d_output_is_shifted;

     protected:
      int calculate_output_stream_length(const gr_vector_int &ninput_items);

     public:
      
      ofdm_carrier_allocator_802_11_impl(
	  int fft_len,
	  const std::vector<std::vector<int> > &occupied_carriers,
	  const std::vector<std::vector<int> > &pilot_carriers,
	  const std::vector<std::vector<gr_complex> > &pilot_symbols,
	  const std::vector<std::vector<gr_complex> > &sync_words,
	  const std::string &len_tag_key,
	  const bool output_is_shifted
      );
      ~ofdm_carrier_allocator_802_11_impl();

      std::string len_tag_key() { return d_length_tag_key_str; };

      const int fft_len() { return d_fft_len; };
      std::vector<std::vector<int> > occupied_carriers() { return d_occupied_carriers; };

      int work(int noutput_items,
		 gr_vector_int &ninput_items,
		 gr_vector_const_void_star &input_items,
		 gr_vector_void_star &output_items);

    };

  } // namespace ieee802-11
} // namespace gr

#endif /* INCLUDED_IEEE802_11_OFDM_CARRIER_ALLOCATOR_802_11_IMPL_H */

