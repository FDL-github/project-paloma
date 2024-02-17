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

#include "encrypt.h"

/**
 * @brief Encryption
 *
 * @param [out] synd_vec syndrome vector
 * @param [in] pk : public key
 * @param [in] err_vec : error vector
 */
void encrypt(OUT Word *synd_vec, IN const PublicKey *pk, IN const Word *err_vec)
{
    Word err_vec_t[PARAM_N_WORDS] = {0};

    /* 항등행렬 부분 행렬곱 */
    memcpy(synd_vec, err_vec, (sizeof(Word)) * SYND_WORDS);

    for (int i = 0; i < PARAM_K_WORDS; i++)
    {
        err_vec_t[i] = err_vec[SYND_WORDS + i];
    }

    Word tmp[PARAM_N_WORDS] = {0};

    /* [M] X e_j */
    matXvec(tmp, (pk)->H, err_vec_t, SYND_BITS, PARAM_K);

    /* [I|M] X e_j */
    for (int i = 0; i < SYND_WORDS; i++)
    {
        synd_vec[i] ^= tmp[i];
    }
}
