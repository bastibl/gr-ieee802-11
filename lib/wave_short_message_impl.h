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
#ifndef INCLUDED_IEEE802_11_WAVE_SHORT_MESSAGE_IMPL_H
#define INCLUDED_IEEE802_11_WAVE_SHORT_MESSAGE_IMPL_H

#include <ieee802-11/wave_short_message.h>

namespace gr {
namespace ieee802_11 {

	struct wave_hdr_v0 {
		uint8_t version;
		uint8_t security;
		uint8_t channel;
		uint8_t rate;
		uint8_t power;
		uint8_t app_class;
		uint8_t acm_length;
		uint16_t length;
	}__attribute__((packed));

	struct wave_hdr_v2 {
		uint8_t version;
		uint8_t psid;
		uint8_t wsm_id;
		uint16_t length;
	}__attribute__((packed));

	class wave_short_message_impl : public wave_short_message {


		public:
			wave_short_message_impl(int version);

		private:
			void make_frame (pmt::pmt_t msg);
			void set_version(int version);

			wave_hdr_v0 hdr0;
			wave_hdr_v2 hdr2;
			int d_version;
	};

} // namespace ieee802_11
} // namespace gr

#endif /* INCLUDED_IEEE802_11_WAVE_SHORT_MESSAGE_IMPL_H */

