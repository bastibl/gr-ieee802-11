/*
 * Copyright (C) 2015 Bastian Bloessl <bloessl@ccs-labs.org>
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

#include "lms.h"
#include <cstring>
#include <iostream>

using namespace gr::ieee802_11::equalizer;

void lms::equalize(const gr_complex *in, gr_complex *out, int n) {

	static float freq_offset = 0;
	static float freq_sum = 0;
	static float alpha = 0.9;

	if(n == 0) {
		std::memcpy(carrier, in, 64 * sizeof(gr_complex));
	} else if(n == 1) {
		gr_complex sum =
			(carrier[11] * conj(in[11])) +
			(carrier[25] * conj(in[25])) +
			(carrier[39] * conj(in[39])) +
			(carrier[53] * conj(in[53]));
		freq_offset = arg(sum);
		freq_sum = 0;
		for(int i = 0; i < 64; i++) {
			carrier[i] += in[i];
			carrier[i] *= ref[i] * gr_complex(0.5, 0);
		}
	} else {

		gr_complex p = POLARITY[(n - 2) % 127];
		gr_complex sum =
			(carrier[11] * conj(in[11]) *  p) +
			(carrier[25] * conj(in[25]) *  p) +
			(carrier[39] * conj(in[39]) *  p) +
			(carrier[53] * conj(in[53]) * -p);

		freq_offset = (1 - alpha) * freq_offset + alpha * arg(sum);
		freq_sum += freq_offset;

		//std::cout << "lms: symbol " << n
		//	<< "   p1 " << abs(carrier[11] * conj(in[11]) *  p)
		//	<< "   p2 " << abs(carrier[25] * conj(in[25]) *  p)
		//	<< "   p3 " << abs(carrier[39] * conj(in[39]) *  p)
		//	<< "   p4 " << abs(carrier[53] * conj(in[53]) * -p)
		//	<< std::endl;
		//std::cout << "lms: symbol " << n
		//	<< "   p1 " << arg(carrier[11] * conj(in[11]) *  p)
		//	<< "   p2 " << arg(carrier[25] * conj(in[25]) *  p)
		//	<< "   p3 " << arg(carrier[39] * conj(in[39]) *  p)
		//	<< "   p4 " << arg(carrier[53] * conj(in[53]) * -p)
		//	<< std::endl;

		carrier[11] = in[11] *  p;
		carrier[25] = in[25] *  p;
		carrier[39] = in[39] *  p;
		carrier[53] = in[53] * -p;

		int c = 0;
		for(int i = 0; i < 64; i++) {
			if( (i == 11) || (i == 25) || (i == 32) || (i == 39) || (i == 53) || (i < 6) || ( i > 58)) {
				continue;
			} else {
				out[c] = in[i] / carrier[i] * exp(gr_complex(0, freq_sum));
				c++;
			}
		}
	}
}

const gr_complex lms::ref[] = {
	 0,  0,  0,  0,  0,  0,  1,  1, -1, -1,
	 1,  1, -1,  1, -1,  1,  1,  1,  1,  1,
	 1, -1, -1,  1,  1, -1,  1, -1,  1,  1,
	 1,  1,  0,  1, -1, -1,  1,  1, -1,  1,
	-1,  1, -1, -1, -1, -1, -1,  1,  1, -1,
	-1,  1, -1,  1, -1,  1,  1,  1,  1,  0,
	 0,  0,  0,  0
};

const gr_complex lms::POLARITY[127] = {
	 1, 1, 1, 1,-1,-1,-1, 1,-1,-1,-1,-1, 1, 1,-1, 1,
	-1,-1, 1, 1,-1, 1, 1,-1, 1, 1, 1, 1, 1, 1,-1, 1,
	 1, 1,-1, 1, 1,-1,-1, 1, 1, 1,-1, 1,-1,-1,-1, 1,
	-1, 1,-1,-1, 1,-1,-1, 1, 1, 1, 1, 1,-1,-1, 1, 1,
	-1,-1, 1,-1, 1,-1, 1, 1,-1,-1,-1, 1, 1,-1,-1,-1,
	-1, 1,-1,-1, 1,-1, 1, 1, 1, 1,-1, 1,-1, 1,-1, 1,
	-1,-1,-1,-1,-1, 1,-1, 1, 1,-1, 1,-1, 1, 1, 1,-1,
	-1, 1,-1,-1,-1, 1, 1, 1,-1,-1,-1,-1,-1,-1,-1 };
