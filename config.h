/*
 * Copyright (c) 2024 FDL(Future cryptography Design Lab.) Kookmin University
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#ifndef CONFIG_H
#define CONFIG_H

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define IN
#define OUT

#ifndef WORD
#define WORD 64
#endif

#if WORD == 8
typedef uint8_t Word;
#define WORD_BITS 8
#define WORD_BYTES 1

#elif WORD == 32
typedef uint32_t Word;
#define WORD_BITS 32
#define WORD_BYTES 4

#elif WORD == 64
typedef uint64_t Word;
#define WORD_BITS 64
#define WORD_BYTES 8

#endif

#define PARAM_M 13
#define SEED_BITS 256
#define SEED_BYTES 256 / 8
#define SEED_WORDS 256 / WORD_BITS

/**
 * @brief Set PALOMA_MODE
 * @brief PALOMA_MODE == 0: PALOMA-128
 * @brief PALOMA_MODE == 1: PALOMA-192
 * @brief PALOMA_MODE == 2: PALOMA-256
 */
#ifndef PALOMA_MODE
#define PALOMA_MODE 2
#endif

#if PALOMA_MODE == 0
#define PARAM_N 3904
#define PARAM_T 64
#define PARAM_K (PARAM_N - (PARAM_M * PARAM_T))

#define PUBLICKEYBYTES 319488
#define SECRETKEYBYTES 94496
#define CIPHERTEXTBYTES 136
#define BYTES 32

#elif PALOMA_MODE == 1
#define PARAM_N 5568
#define PARAM_T 128
#define PARAM_K (PARAM_N - (PARAM_M * PARAM_T))

#define PUBLICKEYBYTES 812032
#define SECRETKEYBYTES 357536
#define CIPHERTEXTBYTES 240
#define BYTES 32

#elif PALOMA_MODE == 2
#define PARAM_N 6592
#define PARAM_T 128
#define PARAM_K (PARAM_N - (PARAM_M * PARAM_T))

#define PUBLICKEYBYTES 1025024
#define SECRETKEYBYTES 359584
#define CIPHERTEXTBYTES 240
#define BYTES 32

#endif

/* all elements of F2m is ' z^12 + ... +a_0z^0 ' > 0 || 0 || 0 || a_12 || a_11 || .... || a_0  : 2bytes */
typedef uint16_t gf;
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

/* Variables for Multiplication Precalculation Tables */
#define GF2M_SPLIT_LOW 7
#define GF2M_SPLIT_HIGH (PARAM_M - GF2M_SPLIT_LOW)            // 6
#define GF2M_SPLIT_MASK_LOW_BIT ((1 << GF2M_SPLIT_LOW) - 1)   // 127
#define GF2M_SPLIT_MASK_HIGH_BIT ((1 << GF2M_SPLIT_HIGH) - 1) // 63

/* Variables for input validation */
#define BITSIZE (1 << PARAM_M)
#define MASKBITS ((1 << PARAM_M) - 1)

/* HP Matrix : mt(=n-k) * n */
#define HP_NROWS (PARAM_M * PARAM_T)                        // HP Matrix's row bit size : mt(= n-k)
#define HP_NROWS_WORDS (PARAM_M * PARAM_T / WORD_BITS)      // HP Matrix's row Word size : mt(=n-k) / Wordbits
#define HP_NCOLS PARAM_N                                    // HP Matrix's column bit size : n
#define HP_NCOLS_WORDS (PARAM_N / WORD_BITS)                // HP Matrix's column Word size : n
#define HP_WORDS (HP_NROWS * HP_NCOLS / WORD_BITS)          // HP Matrix's total Word size
#define HP_NK_WORDS (HP_NROWS * HP_NROWS / WORD_BITS)       // HP Matrix's Word size - Public key(H_hat[n-k:n])'s Word size : mtmt / Wordbits

/* Public Key : mt(=n-k) * k */
#define PK_NROWS (PARAM_M * PARAM_T)                        // Public key(H_hat[n-k:n])'s row bit size : mt(=n-k)
#define PK_NROWS_WORDS (PARAM_M * PARAM_T / WORD_BITS)      // Public key(H_hat[n-k:n])'s row Word size : mt(=n-k) / Wordbits
#define PK_NCOLS PARAM_K                                    // Public key(H_hat[n-k:n])'s column bit size : k
#define PK_WORDS (PK_NROWS * PK_NCOLS / WORD_BITS)          // Public key(H_hat[n-k:n])'s Word size

/* S^{-1} Matrix : mt(=n-k) * mt(=n-k) */
#define S_INV_WORDS (HP_NROWS * HP_NROWS / WORD_BITS)       // S_inv Matrix's Word size : mtmt / Wordbits

/* Syndrome : mt(=n-k) */
#define SYND_BITS (PARAM_N - PARAM_K)                       // Syndrome Vector's bit size : mt(= n-k)
#define SYND_BYTES ((PARAM_N - PARAM_K) / 8)                // Syndrome Vector's byte size : mt(= n-k) / 8
#define SYND_WORDS ((PARAM_N - PARAM_K) / WORD_BITS)        // Syndrome Vector's Word size : mt(= n-k) / Wordbits

/* GF Polynomial : def(gf_poly) = t -> #coef(gf_poly) = t+1 */
#define GF_POLY_LEN (PARAM_T + 1)

/* Variables for PARAM_N, K */
#define PARAM_N_WORDS (PARAM_N / WORD_BITS)
#define PARAM_K_WORDS (PARAM_K / WORD_BITS)

/* ROH Input: e_tilde || r_hat || s_hat */
#define ROH_INPUT_BITS (PARAM_N + SEED_BITS + (PARAM_N - PARAM_K))
#define ROH_INPUT_WORDS ((PARAM_N + SEED_BITS + (PARAM_N - PARAM_K)) / WORD_BITS)

#endif