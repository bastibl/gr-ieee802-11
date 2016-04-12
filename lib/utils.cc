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
#include "utils.h"

#include <cassert>
#include <cstring>
#include <math.h>

// constellations for different modulations (normalized to avg power 1)
const std::complex<double> BPSK_D[2] = {
		std::complex<double>(-1.0, 0.0), std::complex<double>(1.0, 0.0)};


const std::complex<double> QPSK_D[4] = {
		std::complex<double>(-0.7071, -0.7071), std::complex<double>(-0.7071, 0.7071),
		std::complex<double>(+0.7071, -0.7071), std::complex<double>(+0.7071, 0.7071)};


const std::complex<double> QAM16_D[16] = {
		std::complex<double>(-0.9487, -0.9487), std::complex<double>(-0.9487, -0.3162),
		std::complex<double>(-0.9487, 0.9487), std::complex<double>(-0.9487, 0.3162),
		std::complex<double>(-0.3162, -0.9487), std::complex<double>(-0.3162, -0.3162),
		std::complex<double>(-0.3162, 0.9487), std::complex<double>(-0.3162, 0.3162),
		std::complex<double>(0.9487, -0.9487), std::complex<double>(0.9487, -0.3162),
		std::complex<double>(0.9487, 0.9487), std::complex<double>(0.9487, 0.3162),
		std::complex<double>(0.3162, -0.9487), std::complex<double>(0.3162, -0.3162),
		std::complex<double>(0.3162, 0.9487), std::complex<double>(0.3162, 0.3162)};


const std::complex<double> QAM64_D[64] = {
		std::complex<double>(-1.0801, -1.0801), std::complex<double>(-1.0801, -0.7715),
		std::complex<double>(-1.0801, -0.1543), std::complex<double>(-1.0801, -0.4629),
		std::complex<double>(-1.0801, 1.0801), std::complex<double>(-1.0801, 0.7715),
		std::complex<double>(-1.0801, 0.1543), std::complex<double>(-1.0801, 0.4629),
		std::complex<double>(-0.7715, -1.0801), std::complex<double>(-0.7715, -0.7715),
		std::complex<double>(-0.7715, -0.1543), std::complex<double>(-0.7715, -0.4629),
		std::complex<double>(-0.7715, 1.0801), std::complex<double>(-0.7715, 0.7715),
		std::complex<double>(-0.7715, 0.1543), std::complex<double>(-0.7715, 0.4629),
		std::complex<double>(-0.1543, -1.0801), std::complex<double>(-0.1543, -0.7715),
		std::complex<double>(-0.1543, -0.1543), std::complex<double>(-0.1543, -0.4629),
		std::complex<double>(-0.1543, 1.0801), std::complex<double>(-0.1543, 0.7715),
		std::complex<double>(-0.1543, 0.1543), std::complex<double>(-0.1543, 0.4629),
		std::complex<double>(-0.4629, -1.0801), std::complex<double>(-0.4629, -0.7715),
		std::complex<double>(-0.4629, -0.1543), std::complex<double>(-0.4629, -0.4629),
		std::complex<double>(-0.4629, 1.0801), std::complex<double>(-0.4629, 0.7715),
		std::complex<double>(-0.4629, 0.1543), std::complex<double>(-0.4629, 0.4629),
		std::complex<double>(1.0801, -1.0801), std::complex<double>(1.0801, -0.7715),
		std::complex<double>(1.0801, -0.1543), std::complex<double>(1.0801, -0.4629),
		std::complex<double>(1.0801, 1.0801), std::complex<double>(1.0801, 0.7715),
		std::complex<double>(1.0801, 0.1543), std::complex<double>(1.0801, 0.4629),
		std::complex<double>(0.7715, -1.0801), std::complex<double>(0.7715, -0.7715),
		std::complex<double>(0.7715, -0.1543), std::complex<double>(0.7715, -0.4629),
		std::complex<double>(0.7715, 1.0801), std::complex<double>(0.7715, 0.7715),
		std::complex<double>(0.7715, 0.1543), std::complex<double>(0.7715, 0.4629),
		std::complex<double>(0.1543, -1.0801), std::complex<double>(0.1543, -0.7715),
		std::complex<double>(0.1543, -0.1543), std::complex<double>(0.1543, -0.4629),
		std::complex<double>(0.1543, 1.0801), std::complex<double>(0.1543, 0.7715),
		std::complex<double>(0.1543, 0.1543), std::complex<double>(0.1543, 0.4629),
		std::complex<double>(0.4629, -1.0801), std::complex<double>(0.4629, -0.7715),
		std::complex<double>(0.4629, -0.1543), std::complex<double>(0.4629, -0.4629),
		std::complex<double>(0.4629, 1.0801), std::complex<double>(0.4629, 0.7715),
		std::complex<double>(0.4629, 0.1543), std::complex<double>(0.4629, 0.4629)};


ofdm_param::ofdm_param(Encoding e) {
	encoding = e;

	switch(e) {
		case BPSK_1_2:
			n_bpsc = 1;
			n_cbps = 48;
			n_dbps = 24;
			rate_field = 0x0D; // 0b00001101
			break;

		case BPSK_3_4:
			n_bpsc = 1;
			n_cbps = 48;
			n_dbps = 36;
			rate_field = 0x0F; // 0b00001111
			break;

		case QPSK_1_2:
			n_bpsc = 2;
			n_cbps = 96;
			n_dbps = 48;
			rate_field = 0x05; // 0b00000101
			break;

		case QPSK_3_4:
			n_bpsc = 2;
			n_cbps = 96;
			n_dbps = 72;
			rate_field = 0x07; // 0b00000111
			break;

		case QAM16_1_2:
			n_bpsc = 4;
			n_cbps = 192;
			n_dbps = 96;
			rate_field = 0x09; // 0b00001001
			break;

		case QAM16_3_4:
			n_bpsc = 4;
			n_cbps = 192;
			n_dbps = 144;
			rate_field = 0x0B; // 0b00001011
			break;

		case QAM64_2_3:
			n_bpsc = 6;
			n_cbps = 288;
			n_dbps = 192;
			rate_field = 0x01; // 0b00000001
			break;

		case QAM64_3_4:
			n_bpsc = 6;
			n_cbps = 288;
			n_dbps = 216;
			rate_field = 0x03; // 0b00000011
			break;
		defaut:
			assert(false);
			break;
	}
}


void
ofdm_param::print() {
	std::cout << "OFDM Parameters:" << std::endl;
	std::cout << "endcoding :" << encoding << std::endl;
	std::cout << "rate_field :" << (int)rate_field << std::endl;
	std::cout << "n_bpsc :" << n_bpsc << std::endl;
	std::cout << "n_cbps :" << n_cbps << std::endl;
	std::cout << "n_dbps :" << n_dbps << std::endl;
}


tx_param::tx_param(ofdm_param &ofdm, int psdu_length) {

	psdu_size = psdu_length;

	// number of symbols (17-11)
	n_sym = (int) ceil((16 + 8 * psdu_size + 6) / (double) ofdm.n_dbps);

	// number of bits of the data field (17-12)
	n_data = n_sym * ofdm.n_dbps;

	// number of padding bits (17-13)
	n_pad = n_data - (16 + 8 * psdu_size + 6);

	n_encoded_bits = n_sym * ofdm.n_cbps;
}
void
tx_param::print() {
	std::cout << "TX Parameters:" << std::endl;
	std::cout << "psdu_size: " << psdu_size << std::endl;
	std::cout << "n_sym: " << n_sym << std::endl;
	std::cout << "n_data: " << n_data << std::endl;
	std::cout << "n_pad: " << n_pad << std::endl;
}


void scramble(const char *in, char *out, tx_param &tx, char initial_state) {

    int state = initial_state;
    int feedback;

    for (int i = 0; i < tx.n_data; i++) {

	feedback = (!!(state & 64)) ^ (!!(state & 8));
	out[i] = feedback ^ in[i];
	state = ((state << 1) & 0x7e) | feedback;
    }
}


void reset_tail_bits(char *scrambled_data, tx_param &tx) {
	memset(scrambled_data + tx.n_data - tx.n_pad - 6, 0, 6 * sizeof(char));
}


int ones(int n) {
	int sum = 0;
	for(int i = 0; i < 8; i++) {
		if(n & (1 << i)) {
			sum++;
		}
	}
	return sum;
}


void convolutional_encoding(const char *in, char *out, tx_param &tx) {

	int state = 0;

	for(int i = 0; i < tx.n_data; i++) {
		assert(in[i] == 0 || in[i] == 1);
		state = ((state << 1) & 0x7e) | in[i];
		out[i * 2]     = ones(state & 0155) % 2;
		out[i * 2 + 1] = ones(state & 0117) % 2;
	}
}


void puncturing(const char *in, char *out, tx_param &tx, ofdm_param &ofdm) {

	int mod;

	for (int i = 0; i < tx.n_data * 2; i++) {
		switch(ofdm.encoding) {
			case BPSK_1_2:
			case QPSK_1_2:
			case QAM16_1_2:
				*out = in[i];
				out++;
				break;

			case QAM64_2_3:
				if (i % 4 != 3) {
					*out = in[i];
					out++;
				}
				break;

			case BPSK_3_4:
			case QPSK_3_4:
			case QAM16_3_4:
			case QAM64_3_4:
				mod = i % 6;
				if (!(mod == 3 || mod == 4)) {
					*out = in[i];
					out++;
				}
				break;
			defaut:
				assert(false);
				break;
		}
	}
}


void interleave(const char *in, char *out, tx_param &tx, ofdm_param &ofdm, bool reverse) {

	int n_cbps = ofdm.n_cbps;
	int first[n_cbps];
	int second[n_cbps];
	int s = std::max(ofdm.n_bpsc / 2, 1);

	for(int j = 0; j < n_cbps; j++) {
		first[j] = s * (j / s) + ((j + int(floor(16.0 * j / n_cbps))) % s);
	}

	for(int i = 0; i < n_cbps; i++) {
		second[i] = 16 * i - (n_cbps - 1) * int(floor(16.0 * i / n_cbps));
	}

	for(int i = 0; i < tx.n_sym; i++) {
		for(int k = 0; k < n_cbps; k++) {
			if(reverse) {
				out[i * n_cbps + second[first[k]]] = in[i * n_cbps + k];
			} else {
				out[i * n_cbps + k] = in[i * n_cbps + second[first[k]]];
			}
		}
	}
}


void interleave(const double *in, double *out, tx_param &tx, ofdm_param &ofdm, bool reverse) {

	int n_cbps = ofdm.n_cbps;
	int first[n_cbps];
	int second[n_cbps];
	int s = std::max(ofdm.n_bpsc / 2, 1);

	for(int j = 0; j < n_cbps; j++) {
		first[j] = s * (j / s) + ((j + int(floor(16.0 * j / n_cbps))) % s);
	}

	for(int i = 0; i < n_cbps; i++) {
		second[i] = 16 * i - (n_cbps - 1) * int(floor(16.0 * i / n_cbps));
	}

	for(int i = 0; i < tx.n_sym; i++) {
		for(int k = 0; k < n_cbps; k++) {
			if(reverse) {
				out[i * n_cbps + second[first[k]]] = in[i * n_cbps + k];
			} else {
				out[i * n_cbps + k] = in[i * n_cbps + second[first[k]]];
			}
		}
	}
}


void split_symbols(const char *in, char *out, tx_param &tx, ofdm_param &ofdm) {

	int symbols = tx.n_sym * 48;

	for (int i = 0; i < symbols; i++) {
		out[i] = 0;
		for(int k = 0; k < ofdm.n_bpsc; k++) {
			assert(*in == 1 || *in == 0);
			out[i] |= (*in << (ofdm.n_bpsc - k - 1));
			in++;
		}
	}
}


void generate_bits(const char *psdu, char *data_bits, tx_param &tx) {

	// first 16 bits are zero (SERVICE/DATA field)
	memset(data_bits, 0, 16);
	data_bits += 16;

	for(int i = 0; i < tx.psdu_size; i++) {
		for(int b = 0; b < 8; b++) {
			data_bits[i * 8 + b] = !!(psdu[i] & (1 << b));
		}
	}
}
/* Here starts the SSE2 Viterbi convolutional decoder implementation */
/*
 * Copyright 1995 Phil Karn, KA9Q
 * Copyright 2008 Free Software Foundation, Inc.
 * 2014 Added SSE2 implementation Bogdan Diaconescu
 *
 * This file is part of GNU Radio
 *
 * GNU Radio is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * GNU Radio is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GNU Radio; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

/*
 * Viterbi decoder for K=7 rate=1/2 convolutional code
 * Some modifications from original Karn code by Matt Ettus
 * Major modifications by adding SSE2 code by Bogdan Diaconescu 
 */
#include <xmmintrin.h>

/* The two generator polynomials for the NASA Standard K=7 code.
 * Since these polynomials are known to be optimal for this constraint
 * length there is not much point in changing them. But if you do, you
 * will have to regenerate the BUTTERFLY macro calls in viterbi()
 */
#define	POLYA	0x6d
#define	POLYB	0x4f

/* The basic Viterbi decoder operation, called a "butterfly"
 * operation because of the way it looks on a trellis diagram. Each
 * butterfly involves an Add-Compare-Select (ACS) operation on the two nodes
 * where the 0 and 1 paths from the current node merge at the next step of
 * the trellis.
 *
 * The code polynomials are assumed to have 1's on both ends. Given a
 * function encode_state() that returns the two symbols for a given
 * encoder state in the low two bits, such a code will have the following
 * identities for even 'n' < 64:
 *
 * 	encode_state(n) = encode_state(n+65)
 *	encode_state(n+1) = encode_state(n+64) = (3 ^ encode_state(n))
 *
 * Any convolutional code you would actually want to use will have
 * these properties, so these assumptions aren't too limiting.
 *
 * Doing this as a macro lets the compiler evaluate at compile time the
 * many expressions that depend on the loop index and encoder state and
 * emit them as immediate arguments.
 * This makes an enormous difference on register-starved machines such
 * as the Intel x86 family where evaluating these expressions at runtime
 * would spill over into memory.
 */


static union branchtab27 { unsigned char c[32]; __m128i v[2];} Branchtab27_sse2[2];

// Maximum number of traceback bytes
#define TRACEBACK_MAX 24
// Metrics for each state
static unsigned char mmresult[64] __attribute__((aligned(16))) = {0};
// Paths for each state
static unsigned char ppresult[TRACEBACK_MAX][64] __attribute__((aligned(16))) = {{0,0}};
// Position in circular buffer where the current decoded byte is stored
static int store_pos = 0;

#define	BUTTERFLY(i,sym) { \
	int m0,m1,m2,m3;\
	/* ACS for 0 branch */\
	m0 = state[i].metric + mets[sym];	/* 2*i */\
	m1 = state[i+32].metric + mets[3 ^ sym];	/* 2*i + 64 */\
	if(m0 > m1){\
		next[2*i].metric = m0;\
		next[2*i].path = state[i].path << 1;\
	} else {\
		next[2*i].metric = m1;\
		next[2*i].path = (state[i+32].path << 1)|1;\
	}\
	/* ACS for 1 branch */\
	m2 = state[i].metric + mets[3 ^ sym];	/* 2*i + 1 */\
	m3 = state[i+32].metric + mets[sym];	/* 2*i + 65 */\
	if(m2 > m3){\
		next[2*i+1].metric = m2;\
		next[2*i+1].path = state[i].path << 1;\
	} else {\
		next[2*i+1].metric = m3;\
		next[2*i+1].path = (state[i+32].path << 1)|1;\
	}\
}

unsigned char d_Partab[] = {
 0, 1, 1, 0, 1, 0, 0, 1,
 1, 0, 0, 1, 0, 1, 1, 0,
 1, 0, 0, 1, 0, 1, 1, 0,
 0, 1, 1, 0, 1, 0, 0, 1,
 1, 0, 0, 1, 0, 1, 1, 0,
 0, 1, 1, 0, 1, 0, 0, 1,
 0, 1, 1, 0, 1, 0, 0, 1,
 1, 0, 0, 1, 0, 1, 1, 0,
 1, 0, 0, 1, 0, 1, 1, 0,
 0, 1, 1, 0, 1, 0, 0, 1,
 0, 1, 1, 0, 1, 0, 0, 1,
 1, 0, 0, 1, 0, 1, 1, 0,
 0, 1, 1, 0, 1, 0, 0, 1,
 1, 0, 0, 1, 0, 1, 1, 0,
 1, 0, 0, 1, 0, 1, 1, 0,
 0, 1, 1, 0, 1, 0, 0, 1,
 1, 0, 0, 1, 0, 1, 1, 0,
 0, 1, 1, 0, 1, 0, 0, 1,
 0, 1, 1, 0, 1, 0, 0, 1,
 1, 0, 0, 1, 0, 1, 1, 0,
 0, 1, 1, 0, 1, 0, 0, 1,
 1, 0, 0, 1, 0, 1, 1, 0,
 1, 0, 0, 1, 0, 1, 1, 0,
 0, 1, 1, 0, 1, 0, 0, 1,
 0, 1, 1, 0, 1, 0, 0, 1,
 1, 0, 0, 1, 0, 1, 1, 0,
 1, 0, 0, 1, 0, 1, 1, 0,
 0, 1, 1, 0, 1, 0, 0, 1,
 1, 0, 0, 1, 0, 1, 1, 0,
 0, 1, 1, 0, 1, 0, 0, 1,
 0, 1, 1, 0, 1, 0, 0, 1,
 1, 0, 0, 1, 0, 1, 1, 0,
};/* Parity lookup table */

/* Convolutionally encode data into binary symbols */
unsigned char
d_encode(unsigned char *symbols,
       unsigned char *data,
       unsigned int nbytes,
       unsigned char encstate)
{
  int i;

  while(nbytes-- != 0){
    for(i=7;i>=0;i--){
      encstate = (encstate << 1) | ((*data >> i) & 1);
      *symbols++ = d_Partab[encstate & POLYA];
      *symbols++ = d_Partab[encstate & POLYB];
    }
    data++;
  }

  return encstate;
}

/* Viterbi decoder */
int
d_viterbi(unsigned long *metric,	/* Final path metric (returned value) */
	unsigned char *data,	/* Decoded output data */
	unsigned char *symbols,	/* Raw deinterleaved input symbols */
	unsigned int nbits,	/* Number of output bits */
	int mettab[2][256]	/* Metric table, [sent sym][rx symbol] */
	){
  unsigned int bitcnt = 0;
  int mets[4];
  long bestmetric;
  int beststate,i;
  struct viterbi_state state0[64],state1[64],*state,*next;

  state = state0;
  next = state1;

  /* Initialize starting metrics to prefer 0 state */
  state[0].metric = 0;
  for(i=1;i<64;i++)
    state[i].metric = -999999;
  state[0].path = 0;

  for(bitcnt = 0;bitcnt < nbits;bitcnt++){
    /* Read input symbol pair and compute all possible branch
     * metrics
     */
    mets[0] = mettab[0][symbols[0]] + mettab[0][symbols[1]];
    mets[1] = mettab[0][symbols[0]] + mettab[1][symbols[1]];
    mets[2] = mettab[1][symbols[0]] + mettab[0][symbols[1]];
    mets[3] = mettab[1][symbols[0]] + mettab[1][symbols[1]];
    symbols += 2;

BUTTERFLY(0,0); BUTTERFLY(1,1); BUTTERFLY(2,3); BUTTERFLY(3,2); 
BUTTERFLY(4,3); BUTTERFLY(5,2); BUTTERFLY(6,0); BUTTERFLY(7,1); 
BUTTERFLY(8,0); BUTTERFLY(9,1); BUTTERFLY(10,3); BUTTERFLY(11,2); 
BUTTERFLY(12,3); BUTTERFLY(13,2); BUTTERFLY(14,0); BUTTERFLY(15,1); 
BUTTERFLY(16,2); BUTTERFLY(17,3); BUTTERFLY(18,1); BUTTERFLY(19,0); 
BUTTERFLY(20,1); BUTTERFLY(21,0); BUTTERFLY(22,2); BUTTERFLY(23,3); 
BUTTERFLY(24,2); BUTTERFLY(25,3); BUTTERFLY(26,1); BUTTERFLY(27,0); 
BUTTERFLY(28,1); BUTTERFLY(29,0); BUTTERFLY(30,2); BUTTERFLY(31,3);

    /* Swap current and next states */
    if(bitcnt & 1){
      state = state0;
      next = state1;
    } else {
      state = state1;
      next = state0;
    }
    // ETTUS
    //if(bitcnt > nbits-7){
    /* In tail, poison non-zero nodes */
    //for(i=1;i<64;i += 2)
    //	state[i].metric = -9999999;
    //}
    /* Produce output every 8 bits once path memory is full */
    if((bitcnt % 8) == 5 && bitcnt > 32){
      /* Find current best path */
      bestmetric = state[0].metric;
      beststate = 0;
      for(i=1;i<64;i++){
	if(state[i].metric > bestmetric){
	  bestmetric = state[i].metric;
	  beststate = i;
	}
      }
#ifdef	notdef
      printf("metrics[%d] = %d state = %lx\n",beststate,
	     state[beststate].metric,state[beststate].path);
#endif
      *data++ = state[beststate].path >> 24;
    }

  }
  /* Output remaining bits from 0 state */
  // ETTUS  Find best state instead
  bestmetric = state[0].metric;
  beststate = 0;
  for(i=1;i<64;i++){
    if(state[i].metric > bestmetric){
      bestmetric = state[i].metric;
      beststate = i;
    }
  }
  if((i = bitcnt % 8) != 6)
    state[beststate].path <<= 6-i;

  *data++ = state[beststate].path >> 24;
  *data++ = state[beststate].path >> 16;
  *data++ = state[beststate].path >> 8;
  *data = state[beststate].path;
  //printf ("BS = %d\tBSM = %d\tM0 = %d\n",beststate,state[beststate].metric,state[0].metric);
  *metric = state[beststate].metric;
  return 0;
}

void
d_viterbi_chunks_init(struct viterbi_state* state) {
  // Initialize starting metrics to prefer 0 state
  int i;
  state[0].metric = 0;
  state[0].path = 0;
  for(i=1;i<64;i++)
  {
    state[i].metric = 0;//-999999;
    state[i].path = 0;
  }
}

void
d_viterbi_chunks_init_sse2(__m128i *mm0, __m128i *pp0) {
  // Initialize starting metrics to prefer 0 state
  int i, j;

  for (i = 0; i < 4; i++)
  {
    mm0[i] = _mm_setzero_si128();
    pp0[i] = _mm_setzero_si128();
  }

  int polys[2] = { POLYA, POLYB };
  for(i=0; i < 32; i++)
  {
    Branchtab27_sse2[0].c[i] = (polys[0] < 0) ^ d_Partab[(2*i) & abs(polys[0])] ? 1 : 0;
    Branchtab27_sse2[1].c[i] = (polys[1] < 0) ^ d_Partab[(2*i) & abs(polys[1])] ? 1 : 0;
  }

  for (i = 0; i < 64; i++)
  {
    mmresult[i] = 0;
    for (j = 0; j < TRACEBACK_MAX; j++)
      ppresult[j][i] = 0;
  }
}


void
d_viterbi_butterfly8(unsigned char *symbols, int mettab[2][256], struct viterbi_state *state0, struct viterbi_state *state1)
{
  unsigned int bitcnt;
  int mets[4];

  struct viterbi_state *state, *next;
  state = state0;
  next = state1;
  // Operate on 16 symbols (8 bits) at a time
  for(bitcnt = 0;bitcnt < 8;bitcnt++){
    // Read input symbol pair and compute all possible branch metrics
    mets[0] = mettab[0][symbols[0]] + mettab[0][symbols[1]];
    mets[1] = mettab[0][symbols[0]] + mettab[1][symbols[1]];
    mets[2] = mettab[1][symbols[0]] + mettab[0][symbols[1]];
    mets[3] = mettab[1][symbols[0]] + mettab[1][symbols[1]];
    symbols += 2;

    // These macro calls were generated by genbut.c
   BUTTERFLY(0,0); BUTTERFLY(1,1); BUTTERFLY(2,3); BUTTERFLY(3,2); 
BUTTERFLY(4,3); BUTTERFLY(5,2); BUTTERFLY(6,0); BUTTERFLY(7,1); 
BUTTERFLY(8,0); BUTTERFLY(9,1); BUTTERFLY(10,3); BUTTERFLY(11,2); 
BUTTERFLY(12,3); BUTTERFLY(13,2); BUTTERFLY(14,0); BUTTERFLY(15,1); 
BUTTERFLY(16,2); BUTTERFLY(17,3); BUTTERFLY(18,1); BUTTERFLY(19,0); 
BUTTERFLY(20,1); BUTTERFLY(21,0); BUTTERFLY(22,2); BUTTERFLY(23,3); 
BUTTERFLY(24,2); BUTTERFLY(25,3); BUTTERFLY(26,1); BUTTERFLY(27,0); 
BUTTERFLY(28,1); BUTTERFLY(29,0); BUTTERFLY(30,2); BUTTERFLY(31,3);

    // Swap current and next states
    if(bitcnt & 1){
      state = state0;
      next = state1;
    } else {
      state = state1;
      next = state0;
    }
  }
}

void
d_viterbi_butterfly2(unsigned char *symbols, int mettab[2][256], struct viterbi_state *state0, struct viterbi_state *state1)
{
  int mets[4];

  struct viterbi_state *state, *next;
  state = state0;
  next = state1;

  // Operate on 4 symbols (2 bits) at a time

  // Read input symbol pair and compute all possible branch metrics
#if 0
  if (symbols[0] == 2)
  {
  mets[0] = mettab[0][symbols[1]];
  mets[1] = mettab[1][symbols[1]];
  mets[2] = mettab[0][symbols[1]];
  mets[3] = mettab[1][symbols[1]];
  } 
  else if (symbols[1] == 2)
  {
  mets[0] = mettab[0][symbols[0]];
  mets[1] = mettab[1][symbols[0]];
  mets[2] = mettab[0][symbols[0]];
  mets[3] = mettab[1][symbols[0]];
  }
  else
#endif
  {
  mets[0] = mettab[0][symbols[0]] + mettab[0][symbols[1]];
  mets[1] = mettab[0][symbols[0]] + mettab[1][symbols[1]];
  mets[2] = mettab[1][symbols[0]] + mettab[0][symbols[1]];
  mets[3] = mettab[1][symbols[0]] + mettab[1][symbols[1]];
  }

  // These macro calls were generated by genbut.c
BUTTERFLY(0,0); BUTTERFLY(1,1); BUTTERFLY(2,3); BUTTERFLY(3,2); 
BUTTERFLY(4,3); BUTTERFLY(5,2); BUTTERFLY(6,0); BUTTERFLY(7,1); 
BUTTERFLY(8,0); BUTTERFLY(9,1); BUTTERFLY(10,3); BUTTERFLY(11,2); 
BUTTERFLY(12,3); BUTTERFLY(13,2); BUTTERFLY(14,0); BUTTERFLY(15,1); 
BUTTERFLY(16,2); BUTTERFLY(17,3); BUTTERFLY(18,1); BUTTERFLY(19,0); 
BUTTERFLY(20,1); BUTTERFLY(21,0); BUTTERFLY(22,2); BUTTERFLY(23,3); 
BUTTERFLY(24,2); BUTTERFLY(25,3); BUTTERFLY(26,1); BUTTERFLY(27,0); 
BUTTERFLY(28,1); BUTTERFLY(29,0); BUTTERFLY(30,2); BUTTERFLY(31,3);

  state = state1;
  next = state0;

  // Read input symbol pair and compute all possible branch metrics

#if 0
  if (symbols[2] == 2)
  {
  mets[0] = mettab[0][symbols[3]];
  mets[1] = mettab[1][symbols[3]];
  mets[2] = mettab[0][symbols[3]];
  mets[3] = mettab[1][symbols[3]];
  } 
  else if (symbols[3] == 2)
  {
  mets[0] = mettab[0][symbols[2]];
  mets[1] = mettab[1][symbols[2]];
  mets[2] = mettab[0][symbols[2]];
  mets[3] = mettab[1][symbols[2]];
  }
  else
#endif
  {
  mets[0] = mettab[0][symbols[2]] + mettab[0][symbols[3]];
  mets[1] = mettab[0][symbols[2]] + mettab[1][symbols[3]];
  mets[2] = mettab[1][symbols[2]] + mettab[0][symbols[3]];
  mets[3] = mettab[1][symbols[2]] + mettab[1][symbols[3]];
  }

  // These macro calls were generated by genbut.c
  BUTTERFLY(0,0); BUTTERFLY(1,1); BUTTERFLY(2,3); BUTTERFLY(3,2); 
BUTTERFLY(4,3); BUTTERFLY(5,2); BUTTERFLY(6,0); BUTTERFLY(7,1); 
BUTTERFLY(8,0); BUTTERFLY(9,1); BUTTERFLY(10,3); BUTTERFLY(11,2); 
BUTTERFLY(12,3); BUTTERFLY(13,2); BUTTERFLY(14,0); BUTTERFLY(15,1); 
BUTTERFLY(16,2); BUTTERFLY(17,3); BUTTERFLY(18,1); BUTTERFLY(19,0); 
BUTTERFLY(20,1); BUTTERFLY(21,0); BUTTERFLY(22,2); BUTTERFLY(23,3); 
BUTTERFLY(24,2); BUTTERFLY(25,3); BUTTERFLY(26,1); BUTTERFLY(27,0); 
BUTTERFLY(28,1); BUTTERFLY(29,0); BUTTERFLY(30,2); BUTTERFLY(31,3);
}

void
d_viterbi_butterfly2_sse2(unsigned char *symbols, __m128i *mm0, __m128i *mm1, __m128i *pp0, __m128i *pp1)
{
  int i;

  __m128i *metric0, *metric1;
  __m128i *path0, *path1;

  metric0 = mm0;
  path0 = pp0;
  metric1 = mm1;
  path1 = pp1;

  // Operate on 4 symbols (2 bits) at a time

  __m128i m0, m1, m2, m3, decision0, decision1, survivor0, survivor1;
  __m128i metsv, metsvm;
  __m128i shift0, shift1;
  __m128i tmp0, tmp1;
  __m128i sym0v, sym1v;

  sym0v = _mm_set1_epi8(symbols[0]);
  sym1v = _mm_set1_epi8(symbols[1]);

  for (i = 0; i < 2; i++)
  {
    if (symbols[0] == 2)
    {
      metsvm = _mm_xor_si128(Branchtab27_sse2[1].v[i],sym1v);
      metsv = _mm_sub_epi8(_mm_set1_epi8(1),metsvm);
    }
    else if (symbols[1] == 2)
    {
      metsvm = _mm_xor_si128(Branchtab27_sse2[0].v[i],sym0v);
      metsv = _mm_sub_epi8(_mm_set1_epi8(1),metsvm);
    }
    else
    {
      metsvm = _mm_add_epi8(_mm_xor_si128(Branchtab27_sse2[0].v[i],sym0v),_mm_xor_si128(Branchtab27_sse2[1].v[i],sym1v));
      metsv = _mm_sub_epi8(_mm_set1_epi8(2),metsvm);
    }

    m0 = _mm_add_epi8(metric0[i], metsv);
    m1 = _mm_add_epi8(metric0[i+2], metsvm);
    m2 = _mm_add_epi8(metric0[i], metsvm);
    m3 = _mm_add_epi8(metric0[i+2], metsv);

    decision0 = _mm_cmpgt_epi8(_mm_sub_epi8(m0,m1),_mm_setzero_si128());
    decision1 = _mm_cmpgt_epi8(_mm_sub_epi8(m2,m3),_mm_setzero_si128());
    survivor0 = _mm_or_si128(_mm_and_si128(decision0,m0),_mm_andnot_si128(decision0,m1));
    survivor1 = _mm_or_si128(_mm_and_si128(decision1,m2),_mm_andnot_si128(decision1,m3));

    shift0 = _mm_slli_epi16(path0[i], 1);
    shift1 = _mm_slli_epi16(path0[2+i], 1);
    shift1 = _mm_add_epi8(shift1, _mm_set1_epi8(1));

    metric1[2*i] = _mm_unpacklo_epi8(survivor0,survivor1);
    tmp0 = _mm_or_si128(_mm_and_si128(decision0,shift0),_mm_andnot_si128(decision0,shift1));

    metric1[2*i+1] = _mm_unpackhi_epi8(survivor0,survivor1);
    tmp1 = _mm_or_si128(_mm_and_si128(decision1,shift0),_mm_andnot_si128(decision1,shift1));

    path1[2*i] = _mm_unpacklo_epi8(tmp0, tmp1);
    path1[2*i+1] = _mm_unpackhi_epi8(tmp0, tmp1);
  }

  metric0 = mm1;
  path0 = pp1;
  metric1 = mm0;
  path1 = pp0;

  sym0v = _mm_set1_epi8(symbols[2]);
  sym1v = _mm_set1_epi8(symbols[3]);

  for (i = 0; i < 2; i++)
  {
    if (symbols[2] == 2)
    {
      metsvm = _mm_xor_si128(Branchtab27_sse2[1].v[i],sym1v);
      metsv = _mm_sub_epi8(_mm_set1_epi8(1),metsvm);
    }
    else if (symbols[3] == 2)
    {
      metsvm = _mm_xor_si128(Branchtab27_sse2[0].v[i],sym0v);
      metsv = _mm_sub_epi8(_mm_set1_epi8(1),metsvm);
    }
    else
    {
      metsvm = _mm_add_epi8(_mm_xor_si128(Branchtab27_sse2[0].v[i],sym0v),_mm_xor_si128(Branchtab27_sse2[1].v[i],sym1v));
      metsv = _mm_sub_epi8(_mm_set1_epi8(2),metsvm);
    }

    m0 = _mm_add_epi8(metric0[i], metsv);
    m1 = _mm_add_epi8(metric0[i+2], metsvm);
    m2 = _mm_add_epi8(metric0[i], metsvm);
    m3 = _mm_add_epi8(metric0[i+2], metsv);

    decision0 = _mm_cmpgt_epi8(_mm_sub_epi8(m0,m1),_mm_setzero_si128());
    decision1 = _mm_cmpgt_epi8(_mm_sub_epi8(m2,m3),_mm_setzero_si128());
    survivor0 = _mm_or_si128(_mm_and_si128(decision0,m0),_mm_andnot_si128(decision0,m1));
    survivor1 = _mm_or_si128(_mm_and_si128(decision1,m2),_mm_andnot_si128(decision1,m3));

    shift0 = _mm_slli_epi16(path0[i], 1);
    shift1 = _mm_slli_epi16(path0[2+i], 1);
    shift1 = _mm_add_epi8(shift1, _mm_set1_epi8(1));

    metric1[2*i] = _mm_unpacklo_epi8(survivor0,survivor1);
    tmp0 = _mm_or_si128(_mm_and_si128(decision0,shift0),_mm_andnot_si128(decision0,shift1));

    metric1[2*i+1] = _mm_unpackhi_epi8(survivor0,survivor1);
    tmp1 = _mm_or_si128(_mm_and_si128(decision1,shift0),_mm_andnot_si128(decision1,shift1));

    path1[2*i] = _mm_unpacklo_epi8(tmp0, tmp1);
    path1[2*i+1] = _mm_unpackhi_epi8(tmp0, tmp1);
  }
}

void
d_viterbi_butterfly_sse2(unsigned char *symbols, __m128i *mm0, __m128i *mm1, __m128i *pp0, __m128i *pp1)
{
  int i;

  __m128i *metric0, *metric1;
  __m128i *path0, *path1;

  metric0 = mm0;
  path0 = pp0;
  metric1 = mm1;
  path1 = pp1;

  for (i = 0; i < 2; i++)
	  printf("symbols[%i]: %i\n", i, symbols[i]);


  // Operate on 4 symbols (2 bits) at a time

  __m128i m0, m1, m2, m3, decision0, decision1, survivor0, survivor1;
  __m128i metsv, metsvm;
  __m128i shift0, shift1;
  __m128i tmp0, tmp1;
  __m128i sym0v, sym1v;

  sym0v = _mm_set1_epi8(symbols[0]);
  sym1v = _mm_set1_epi8(symbols[1]);

  for (i = 0; i < 2; i++)
  {
#if 1
	  if (symbols[0] == 2)
	  {
    metsvm = _mm_xor_si128(Branchtab27_sse2[1].v[i],sym1v);
	  }
	  else if (symbols[1] == 0)
	  {
    metsvm = _mm_xor_si128(Branchtab27_sse2[0].v[i],sym0v);
	  }
	  else
#endif
    metsvm = _mm_add_epi8(_mm_xor_si128(Branchtab27_sse2[0].v[i],sym0v),_mm_xor_si128(Branchtab27_sse2[1].v[i],sym1v));

    metsv = _mm_sub_epi8(_mm_set1_epi8(2),metsvm);

    m0 = _mm_add_epi8(metric0[i], metsv);
    m1 = _mm_add_epi8(metric0[i+2], metsvm);
    m2 = _mm_add_epi8(metric0[i], metsvm);
    m3 = _mm_add_epi8(metric0[i+2], metsv);

    decision0 = _mm_cmpgt_epi8(_mm_sub_epi8(m0,m1),_mm_setzero_si128());
    decision1 = _mm_cmpgt_epi8(_mm_sub_epi8(m2,m3),_mm_setzero_si128());
    survivor0 = _mm_or_si128(_mm_and_si128(decision0,m0),_mm_andnot_si128(decision0,m1));
    survivor1 = _mm_or_si128(_mm_and_si128(decision1,m2),_mm_andnot_si128(decision1,m3));

    shift0 = _mm_slli_epi16(path0[i], 1);
    shift1 = _mm_slli_epi16(path0[2+i], 1);
    shift1 = _mm_add_epi8(shift1, _mm_set1_epi8(1));

    metric1[2*i] = _mm_unpacklo_epi8(survivor0,survivor1);
    tmp0 = _mm_or_si128(_mm_and_si128(decision0,shift0),_mm_andnot_si128(decision0,shift1));

    metric1[2*i+1] = _mm_unpackhi_epi8(survivor0,survivor1);
    tmp1 = _mm_or_si128(_mm_and_si128(decision1,shift0),_mm_andnot_si128(decision1,shift1));

    path1[2*i] = _mm_unpacklo_epi8(tmp0, tmp1);
    path1[2*i+1] = _mm_unpackhi_epi8(tmp0, tmp1);
  }

  for (i = 0; i < 4; i++)
  {
    mm0[i] = metric1[i];
    pp0[i] = path1[i];
  }

}


unsigned char
d_viterbi_get_output(struct viterbi_state *state, unsigned char *outbuf) {
  // Produce output every 8 bits once path memory is full
  //  if((bitcnt % 8) == 5 && bitcnt > 32) {

  //  Find current best path
  unsigned int i, beststate = 0;
  int bestmetric;

  bestmetric = state[0].metric;
  beststate = 0;
  for(i=1;i<64;i++)
  {
    if(state[i].metric > bestmetric) {
      bestmetric = state[i].metric;
      beststate = i;
    }
  }

  *outbuf = state[beststate].path >> 24;

  return bestmetric;
}

unsigned char
d_viterbi_get_output_sse2(__m128i *mm0, __m128i *pp0, int ntraceback, unsigned char *outbuf) {
  //  Find current best path
  int i;
  int bestmetric, minmetric;
  int beststate = 0;
  int pos = 0;

  // Implement a circular buffer with the last ntraceback paths
  store_pos = (store_pos + 1) % ntraceback;

  // TODO - find another way to extract the value
  for (i = 0; i < 4; i++)
  {
    _mm_store_si128((__m128i *) &mmresult[i*16], mm0[i]);
    _mm_store_si128((__m128i *) &ppresult[store_pos][i*16], pp0[i]);
  }

  // Find out the best final state
  bestmetric = mmresult[beststate];
  minmetric = mmresult[beststate];

  for (i = 1; i < 64; i++)
  {
    if (mmresult[i] > bestmetric)
    {
      bestmetric = mmresult[i];
      beststate = i;
    }
    if (mmresult[i] < minmetric)
      minmetric = mmresult[i];
  }

  // Trace back
  for (i = 0, pos = store_pos; i < (ntraceback - 1); i++)
  {
    // Obtain the state from the output bits
    // by clocking in the output bits in reverse order.
    // The state has only 6 bits
    beststate = ppresult[pos][beststate] >> 2;
    pos = (pos - 1 + ntraceback) % ntraceback;
  }

  // Store output byte
  *outbuf = ppresult[pos][beststate];

  // Zero out the path variable
  // and prevent metric overflow
  for (i = 0; i < 4; i++)
  {
    pp0[i] = _mm_setzero_si128();
    mm0[i] = _mm_sub_epi8(mm0[i], _mm_set1_epi8(minmetric));
  }

  return bestmetric;
}
/* end of new functions */
