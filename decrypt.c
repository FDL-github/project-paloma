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

#include "decrypt.h"
/**
 * @brief Decryption
 *
 * @param [out] e_hat Error vector hat.
 * @param [in] sk Secret key.
 * @param [in] s_hat Syndrome vector hat.
 * @param [in] gf2m_tables GF(2^m) operation tables.
 */
void decrypt(OUT Word *e_hat, IN const SecretKey *sk, IN const Word *s_hat, IN const gf2m_tab *gf2m_tables)
{
    Word synd_vec[SYND_WORDS] = {0};
    Word err_vec[PARAM_N_WORDS] = {0};
    gf P[PARAM_N] = {0};
    gf P_inv[PARAM_N] = {0};
    const Word one = 1;

    /* s = S^{-1} * s_hat */
    matXvec(synd_vec, sk->S_inv, s_hat, SYND_BITS, SYND_BITS);

    /* Recover the error vector (using extended Patterson decoding) */
    rec_err_vec(err_vec, sk, synd_vec, gf2m_tables);

    /* Generate the permutation matrix */
    gen_rand_perm_mat(P, P_inv, PARAM_N, sk->r);

    /* Compute e_hat = P^{−1} * e */
    memset(e_hat, 0, (sizeof(Word)) * PARAM_N_WORDS);
    for (size_t i = 0; i < PARAM_N; i++)
    {
        Word bit = ((err_vec[P_inv[i] / WORD_BITS] >> (P_inv[i] % WORD_BITS)) & 1);
        e_hat[i / WORD_BITS] ^= bit << (i % WORD_BITS);
    }
}
