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

#include "encap.h"

/**
 * @brief Function to generate error vector
 *
 * @param [out] err_vec error vector
 * @param [in] seed 256-bit seed r
 */
void gen_rand_err_vec(OUT Word *err_vec, IN const Word *seed)
{
    gf err[PARAM_N] = {0};
    const Word One = 1;

    shuffle(err, PARAM_N, seed);

    for (int i = 0; i < PARAM_T; i++)
    {
        err_vec[err[i] / WORD_BITS] |= (One << (err[i] % WORD_BITS));
    }
}

/**
 * @brief Encapsulation
 *
 * @param [out] key 256-bit key
 * @param [out] r_hat 256-bit string (c = (r_hat, s_hat))
 * @param [out] s_hat syndrome (c = (r_hat, s_hat))
 * @param [in] pk a public key pk
 */
void encap(OUT Word *key, OUT Word *r_hat, OUT Word *s_hat, IN const PublicKey *pk)
{
    Word r_star[SEED_WORDS] = {0};
    Word e_star[PARAM_N_WORDS] = {0};
    Word e_hat[PARAM_N_WORDS] = {0};
    Word e_r_s[ROH_INPUT_WORDS] = {0};

    gf perm_mat_P[PARAM_N] = {0};
    gf perm_mat_P_inv[PARAM_N] = {0};

    /* generate 256-bit seed r_star for error vector */
    gen_rand_sequence(r_star, SEED_BITS);

    /* generate random error vector e_star such that W_H(e_star) = t */
    gen_rand_err_vec(e_star, r_star);

    /* generate 256-bit seed r_hat for permutation matrix */
    /* r_hat = ROG(e_star) */
    int check = rand_oracle(r_hat, e_star, PARAM_N, 1);

    /* generate random permutation matrix to use seed r_hat */
    gen_rand_perm_mat(perm_mat_P, perm_mat_P_inv, PARAM_N, r_hat);

    /* e_hat = P * e_star */
    memset(e_hat, 0, (sizeof(Word)) * PARAM_N_WORDS);
    for (size_t i = 0; i < PARAM_N; i++)
    {
        Word bit = ((e_star[perm_mat_P[i] / WORD_BITS] >> (perm_mat_P[i] % WORD_BITS)) & 1);
        e_hat[i / WORD_BITS] ^= bit << (i % WORD_BITS);
    }

#if BENCH_MODE == ON
    uint64_t cyc_s;
    uint64_t cyc_e;
    cyc_s = cycle_read();
#endif

    /* encrypt */
    encrypt(s_hat, pk, e_hat);

#if BENCH_MODE == ON
    cyc_e = cycle_read();
    paloma_cycle_result.encrypt = cyc_e - cyc_s;
#endif

    /* key k = ROH(e_star, r_hat, s_hat) */
    memcpy(e_r_s, e_star, (sizeof(Word)) * PARAM_N_WORDS);

    memcpy(e_r_s + PARAM_N_WORDS, r_hat, (sizeof(Word)) * SEED_WORDS);

    memcpy(e_r_s + PARAM_N_WORDS + SEED_WORDS, s_hat, (sizeof(Word)) * SYND_WORDS);

    int check2 = rand_oracle(key, e_r_s, ROH_INPUT_BITS, 2);
}