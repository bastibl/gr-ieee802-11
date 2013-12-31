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
#include <boost/format.hpp>
#include <gnuradio/io_signature.h>
#include "packet_headergenerator_impl.h"

using namespace gr::ieee802_11;

packet_headergenerator::sptr packet_headergenerator::make(
		const digital::packet_header_default::sptr &header_formatter,
		const std::string &len_tag_key) {
	return gnuradio::get_initial_sptr (new packet_headergenerator_impl(header_formatter, len_tag_key));
}


packet_headergenerator::sptr packet_headergenerator::make(
		long header_len,
		const std::string &len_tag_key) {
	const digital::packet_header_default::sptr header_formatter(
		new digital::packet_header_default(header_len, len_tag_key));
	return gnuradio::get_initial_sptr (new packet_headergenerator_impl(header_formatter, len_tag_key));
}


    packet_headergenerator_impl::packet_headergenerator_impl(
		    const gr::digital::packet_header_default::sptr &header_formatter,
		    const std::string &len_tag_key
		    )
      : tagged_stream_block("packet_headergenerator_impl",
		       io_signature::make(1, 1, sizeof (char)),
		       io_signature::make(1, 1, sizeof (char)),
		       len_tag_key),
	    d_formatter(header_formatter)
    {
      set_output_multiple(d_formatter->header_len());
      // This is the worst case rate, because we don't know the true value, of course
      set_relative_rate(d_formatter->header_len());
      set_tag_propagation_policy(TPP_DONT);
    }

    packet_headergenerator_impl::~packet_headergenerator_impl()
    {
    }

    int
    packet_headergenerator_impl::work (int noutput_items,
                       gr_vector_int &ninput_items,
                       gr_vector_const_void_star &input_items,
                       gr_vector_void_star &output_items)
    {
      unsigned char *out = (unsigned char *) output_items[0];
      std::vector<tag_t> all_tags;
      get_tags_in_range(all_tags, 0, nitems_read(0), nitems_read(0) + ninput_items[0]);
      if (!d_formatter->header_formatter(ninput_items[0], out, all_tags)) {
	GR_LOG_FATAL(d_logger, boost::format("header_formatter() returned false (this shouldn't happen). Offending header started at %1%") % nitems_read(0));
	throw std::runtime_error("header formatter returned false.");
      }

      return d_formatter->header_len();
    }

