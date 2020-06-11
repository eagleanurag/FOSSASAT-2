/* General purpose Reed-Solomon codec for 8-bit symbols or less
 * Copyright 2003 Phil Karn, KA9Q
 * May be used under the terms of the GNU Lesser General Public License (LGPL)
 *
 * This version tweaked by Philip Heron <phil@sanslogic.co.uk>
*/

#include <string.h>
#include "rs8.h"

static const uint8_t ALPHA_TO[] = {
0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80,0x87,0x89,0x95,0xAD,0xDD,0x3D,0x7A,0xF4,
0x6F,0xDE,0x3B,0x76,0xEC,0x5F,0xBE,0xFB,0x71,0xE2,0x43,0x86,0x8B,0x91,0xA5,0xCD,
0x1D,0x3A,0x74,0xE8,0x57,0xAE,0xDB,0x31,0x62,0xC4,0x0F,0x1E,0x3C,0x78,0xF0,0x67,
0xCE,0x1B,0x36,0x6C,0xD8,0x37,0x6E,0xDC,0x3F,0x7E,0xFC,0x7F,0xFE,0x7B,0xF6,0x6B,
0xD6,0x2B,0x56,0xAC,0xDF,0x39,0x72,0xE4,0x4F,0x9E,0xBB,0xF1,0x65,0xCA,0x13,0x26,
0x4C,0x98,0xB7,0xE9,0x55,0xAA,0xD3,0x21,0x42,0x84,0x8F,0x99,0xB5,0xED,0x5D,0xBA,
0xF3,0x61,0xC2,0x03,0x06,0x0C,0x18,0x30,0x60,0xC0,0x07,0x0E,0x1C,0x38,0x70,0xE0,
0x47,0x8E,0x9B,0xB1,0xE5,0x4D,0x9A,0xB3,0xE1,0x45,0x8A,0x93,0xA1,0xC5,0x0D,0x1A,
0x34,0x68,0xD0,0x27,0x4E,0x9C,0xBF,0xF9,0x75,0xEA,0x53,0xA6,0xCB,0x11,0x22,0x44,
0x88,0x97,0xA9,0xD5,0x2D,0x5A,0xB4,0xEF,0x59,0xB2,0xE3,0x41,0x82,0x83,0x81,0x85,
0x8D,0x9D,0xBD,0xFD,0x7D,0xFA,0x73,0xE6,0x4B,0x96,0xAB,0xD1,0x25,0x4A,0x94,0xAF,
0xD9,0x35,0x6A,0xD4,0x2F,0x5E,0xBC,0xFF,0x79,0xF2,0x63,0xC6,0x0B,0x16,0x2C,0x58,
0xB0,0xE7,0x49,0x92,0xA3,0xC1,0x05,0x0A,0x14,0x28,0x50,0xA0,0xC7,0x09,0x12,0x24,
0x48,0x90,0xA7,0xC9,0x15,0x2A,0x54,0xA8,0xD7,0x29,0x52,0xA4,0xCF,0x19,0x32,0x64,
0xC8,0x17,0x2E,0x5C,0xB8,0xF7,0x69,0xD2,0x23,0x46,0x8C,0x9F,0xB9,0xF5,0x6D,0xDA,
0x33,0x66,0xCC,0x1F,0x3E,0x7C,0xF8,0x77,0xEE,0x5B,0xB6,0xEB,0x51,0xA2,0xC3,0x00,
};

static const uint8_t INDEX_OF[] = {
0xFF,0x00,0x01,0x63,0x02,0xC6,0x64,0x6A,0x03,0xCD,0xC7,0xBC,0x65,0x7E,0x6B,0x2A,
0x04,0x8D,0xCE,0x4E,0xC8,0xD4,0xBD,0xE1,0x66,0xDD,0x7F,0x31,0x6C,0x20,0x2B,0xF3,
0x05,0x57,0x8E,0xE8,0xCF,0xAC,0x4F,0x83,0xC9,0xD9,0xD5,0x41,0xBE,0x94,0xE2,0xB4,
0x67,0x27,0xDE,0xF0,0x80,0xB1,0x32,0x35,0x6D,0x45,0x21,0x12,0x2C,0x0D,0xF4,0x38,
0x06,0x9B,0x58,0x1A,0x8F,0x79,0xE9,0x70,0xD0,0xC2,0xAD,0xA8,0x50,0x75,0x84,0x48,
0xCA,0xFC,0xDA,0x8A,0xD6,0x54,0x42,0x24,0xBF,0x98,0x95,0xF9,0xE3,0x5E,0xB5,0x15,
0x68,0x61,0x28,0xBA,0xDF,0x4C,0xF1,0x2F,0x81,0xE6,0xB2,0x3F,0x33,0xEE,0x36,0x10,
0x6E,0x18,0x46,0xA6,0x22,0x88,0x13,0xF7,0x2D,0xB8,0x0E,0x3D,0xF5,0xA4,0x39,0x3B,
0x07,0x9E,0x9C,0x9D,0x59,0x9F,0x1B,0x08,0x90,0x09,0x7A,0x1C,0xEA,0xA0,0x71,0x5A,
0xD1,0x1D,0xC3,0x7B,0xAE,0x0A,0xA9,0x91,0x51,0x5B,0x76,0x72,0x85,0xA1,0x49,0xEB,
0xCB,0x7C,0xFD,0xC4,0xDB,0x1E,0x8B,0xD2,0xD7,0x92,0x55,0xAA,0x43,0x0B,0x25,0xAF,
0xC0,0x73,0x99,0x77,0x96,0x5C,0xFA,0x52,0xE4,0xEC,0x5F,0x4A,0xB6,0xA2,0x16,0x86,
0x69,0xC5,0x62,0xFE,0x29,0x7D,0xBB,0xCC,0xE0,0xD3,0x4D,0x8C,0xF2,0x1F,0x30,0xDC,
0x82,0xAB,0xE7,0x56,0xB3,0x93,0x40,0xD8,0x34,0xB0,0xEF,0x26,0x37,0x0C,0x11,0x44,
0x6F,0x78,0x19,0x9A,0x47,0x74,0xA7,0xC1,0x23,0x53,0x89,0xFB,0x14,0x5D,0xF8,0x97,
0x2E,0x4B,0xB9,0x60,0x0F,0xED,0x3E,0xE5,0xF6,0x87,0xA5,0x17,0x3A,0xA3,0x3C,0xB7,
};

static const uint8_t GENPOLY[] = {
0x00,0xF9,0x3B,0x42,0x04,0x2B,0x7E,0xFB,0x61,0x1E,0x03,0xD5,0x32,0x42,0xAA,0x05,
0x18,0x05,0xAA,0x42,0x32,0xD5,0x03,0x1E,0x61,0xFB,0x7E,0x2B,0x04,0x42,0x3B,0xF9,
0x00,
};

static inline int mod255(int x)
{
	while(x >= 255)
	{
		x -= 255;
		x = (x >> 8) + (x & 255);
	}
	return(x);
}
#define MODNN(x) mod255(x)

#define MM     (8)
#define NN     (255)
#define NROOTS (32)
#define FCR    (112)
#define PRIM   (11)
#define IPRIM  (116)

#define MIN(a,b) ((a) < (b) ? (a) : (b))
#define A0       (NN) /* Special reserved value encoding zero in index form */

/* Portable C version */
void encode_rs_8(uint8_t *data, uint8_t *parity, int pad)
{
	int i, j;

  // cppcheck-suppress variableScope
	uint8_t feedback;

	memset(parity, 0, NROOTS * sizeof(uint8_t));

	for(i = 0; i < NN - NROOTS - pad; i++)
	{
		feedback = INDEX_OF[data[i] ^ parity[0]];
		if(feedback != A0) /* feedback term is non-zero */
		{
			for(j = 1; j < NROOTS; j++)
				parity[j] ^= ALPHA_TO[mod255(feedback + GENPOLY[NROOTS - j])];
		}

		/* Shift */
		memmove(&parity[0], &parity[1], sizeof(uint8_t) * (NROOTS - 1));
		if(feedback != A0)
			parity[NROOTS - 1] = ALPHA_TO[mod255(feedback + GENPOLY[0])];
		else
			parity[NROOTS - 1] = 0;
	}
}

// cppcheck-suppress unusedFunction
int decode_rs_8(uint8_t *data, int *eras_pos, int no_eras, int pad)
{
	int deg_lambda, el, deg_omega;
	int i, j, r, k;
	uint8_t u, q, tmp, num1, num2, den, discr_r;
	uint8_t lambda[NROOTS + 1], s[NROOTS]; /* Err+Eras Locator poly
	                                        * and syndrome poly */
	uint8_t b[NROOTS + 1], t[NROOTS + 1], omega[NROOTS + 1];
	uint8_t root[NROOTS], reg[NROOTS + 1], loc[NROOTS];
	int syn_error, count;

	if(pad < 0 || pad > 222) return(-1);

	/* form the syndromes; i.e., evaluate data(x) at roots of g(x) */
	for(i = 0; i < NROOTS; i++) s[i] = data[0];

	for(j = 1; j < NN - pad; j++)
	{
		for(i = 0; i < NROOTS; i++)
		{
			if(s[i] == 0) s[i] = data[j];
			else s[i] = data[j] ^ ALPHA_TO[MODNN(INDEX_OF[s[i]] + (FCR + i) * PRIM)];
		}
	}

	/* Convert syndromes to index form, checking for nonzero condition */
	syn_error = 0;
	for(i = 0; i < NROOTS; i++)
	{
		syn_error |= s[i];
		s[i] = INDEX_OF[s[i]];
	}

	if(!syn_error)
	{
		/* if syndrome is zero, data[] is a codeword and there are no
		 * errors to correct. So return data[] unmodified
		 */
		count = 0;
		goto finish;
	}

	memset(&lambda[1], 0, NROOTS * sizeof(lambda[0]));
	lambda[0] = 1;

	if(no_eras > 0)
	{
		/* Init lambda to be the erasure locator polynomial */
		lambda[1] = ALPHA_TO[MODNN(PRIM * (NN - 1 - eras_pos[0]))];
		for(i = 1; i < no_eras; i++)
		{
			u = MODNN(PRIM * (NN - 1 - eras_pos[i]));
			for(j = i + 1; j > 0; j--)
			{
				tmp = INDEX_OF[lambda[j - 1]];
				if(tmp != A0) lambda[j] ^= ALPHA_TO[MODNN(u + tmp)];
			}
		}

	}

	for(i = 0; i < NROOTS + 1; i++)
		b[i] = INDEX_OF[lambda[i]];

	/*
	 * Begin Berlekamp-Massey algorithm to determine error+erasure
	 * locator polynomial
	 */
	r = no_eras;
	el = no_eras;
	while(++r <= NROOTS) /* r is the step number */
	{
		/* Compute discrepancy at the r-th step in poly-form */
		discr_r = 0;
		for(i = 0; i < r; i++)
		{
			if((lambda[i] != 0) && (s[r - i - 1] != A0))
			{
				discr_r ^= ALPHA_TO[MODNN(INDEX_OF[lambda[i]] + s[r - i - 1])];
			}
		}
		discr_r = INDEX_OF[discr_r]; /* Index form */
		if(discr_r == A0)
		{
			/* 2 lines below: B(x) <-- x*B(x) */
			memmove(&b[1], b, NROOTS * sizeof(b[0]));
			b[0] = A0;
		}
		else
		{
			/* 7 lines below: T(x) <-- lambda(x) - discr_r*x*b(x) */
			t[0] = lambda[0];
			for(i = 0; i < NROOTS; i++)
			{
				if(b[i] != A0)
					t[i + 1] = lambda[i + 1] ^ ALPHA_TO[MODNN(discr_r + b[i])];
				else
					t[i + 1] = lambda[i + 1];
			}

			if(2 * el <= r + no_eras - 1)
			{
				el = r + no_eras - el;
				/*
				 * 2 lines below: B(x) <-- inv(discr_r) *
				 * lambda(x)
				 */
				for(i = 0; i <= NROOTS; i++)
					b[i] = (lambda[i] == 0) ? A0 : MODNN(INDEX_OF[lambda[i]] - discr_r + NN);
			}
			else
			{
				/* 2 lines below: B(x) <-- x*B(x) */
				memmove(&b[1], b, NROOTS * sizeof(b[0]));
				b[0] = A0;
			}

			memcpy(lambda, t, (NROOTS + 1) * sizeof(t[0]));
		}
	}

	/* Convert lambda to index form and compute deg(lambda(x)) */
	deg_lambda = 0;
	for(i = 0; i < NROOTS + 1; i++)
	{
		lambda[i] = INDEX_OF[lambda[i]];
		if(lambda[i] != A0) deg_lambda = i;
	}

	/* Find roots of the error+erasure locator polynomial by Chien search */
	memcpy(&reg[1], &lambda[1], NROOTS * sizeof(reg[0]));
	count = 0; /* Number of roots of lambda(x) */
	for(i = 1, k = IPRIM - 1; i <= NN; i++, k = MODNN(k + IPRIM))
	{
		q = 1; /* lambda[0] is always 0 */
		for(j = deg_lambda; j > 0; j--)
		{
			if(reg[j] != A0)
			{
				reg[j] = MODNN(reg[j] + j);
				q ^= ALPHA_TO[reg[j]];
			}
		}

		if (q != 0) continue; /* Not a root */

		/* store root (index-form) and error location number */
		root[count] = i;
		loc[count] = k;
		/* If we've already found max possible roots,
		 * abort the search to save time
		 */
		if(++count == deg_lambda) break;
	}

	if(deg_lambda != count)
	{
		/*
		 * deg(lambda) unequal to number of roots => uncorrectable
		 * error detected
		 */
		count = -1;
		goto finish;
	}

	/*
	 * Compute err+eras evaluator poly omega(x) = s(x)*lambda(x) (modulo
	 * x**NROOTS). in index form. Also find deg(omega).
	 */
	deg_omega = deg_lambda - 1;
	for(i = 0; i <= deg_omega; i++)
	{
		tmp = 0;
		for(j = i; j >= 0; j--)
		{
			if((s[i - j] != A0) && (lambda[j] != A0))
				tmp ^= ALPHA_TO[MODNN(s[i - j] + lambda[j])];
		}
		omega[i] = INDEX_OF[tmp];
	}

	/*
	 * Compute error values in poly-form. num1 = omega(inv(X(l))), num2 =
	 * inv(X(l))**(FCR-1) and den = lambda_pr(inv(X(l))) all in poly-form
	 */
	for(j = count - 1; j >= 0; j--)
	{
		num1 = 0;
		for(i = deg_omega; i >= 0; i--)
		{
			if(omega[i] != A0) num1 ^= ALPHA_TO[MODNN(omega[i] + i * root[j])];
		}
		num2 = ALPHA_TO[MODNN(root[j] * (FCR - 1) + NN)];
		den = 0;

		/* lambda[i+1] for i even is the formal derivative lambda_pr of lambda[i] */
		for(i = MIN(deg_lambda, NROOTS - 1) & ~1; i >= 0; i -= 2)
		{
			if(lambda[i + 1] != A0) den ^= ALPHA_TO[MODNN(lambda[i + 1] + i * root[j])];
		}

		/* Apply error to data */
		if(num1 != 0 && loc[j] >= pad)
		{
			data[loc[j] - pad] ^= ALPHA_TO[MODNN(INDEX_OF[num1] + INDEX_OF[num2] + NN - INDEX_OF[den])];
		}
	}

finish:
	if(eras_pos != NULL)
	{
		for(i = 0; i < count; i++) eras_pos[i] = loc[i];
	}

	return(count);
}
