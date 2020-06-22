/*
 * Copyright (C) 2016 Bastian Bloessl <bloessl@ccs-labs.org>
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

#include "ls.h"
#include <cstring>
#include <iostream>
#include <map>

using namespace gr::ieee802_11::equalizer;

double * ls::equalize(gr_complex *in, int n, gr_complex *symbols, uint8_t *bits, boost::shared_ptr<gr::digital::constellation> mod) {
	double signals[52];
	double noises[52];

	if(n == 0) {
		std::memcpy(d_H, in, 64 * sizeof(gr_complex));

	} else if(n == 1) {
		double signal = 0;
		double noise = 0;
		double* signals = new double[52];
		double* noises = new double[52];
		for(int i = 0; i < 64; i++) {
			if((i == 32) || (i < 6) || ( i > 58)) {
				continue;
			}
			noise += std::pow(std::abs(d_H[i] - in[i]), 2);
			noises[i] = noise;
			signal += std::pow(std::abs(d_H[i] + in[i]), 2);
			signals[i] = signal;
			d_H[i] += in[i];
			d_H[i] /= LONG[i] * gr_complex(2, 0);
		}

		d_snr = 10 * std::log10(signal / noise / 2);

	} else {

		int c = 0;
		for(int i = 0; i < 64; i++) {
			if( (i == 11) || (i == 25) || (i == 32) || (i == 39) || (i == 53) || (i < 6) || ( i > 58)) {
				continue;
			} else {
				symbols[c] = in[i] / d_H[i];
				bits[c] = mod->decision_maker(&symbols[c]);
				c++;
			}
		}
	}

	return signals, noises;
}

double ls::get_snr() {
	return d_snr;
}
