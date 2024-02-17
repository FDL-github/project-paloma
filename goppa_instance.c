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

#include "goppa_instance.h"

/**
 * @brief generate goppa polynomial (x-a_1)(x-a_2)...(x-a_t).
 *
 * @param [out] gx goppa polynomial.
 * @param [in] gf_set number t GF(2^m) elements.
 * @param [in] gf2m_tables tables for efficient arithmetic over GF(2^m).
 */
void gen_goppa_poly(OUT gf *gx, IN const gf *gf_set, IN const gf2m_tab *gf2m_tables)
{
    gf t_gx[GF_POLY_LEN] = {0}; // gx_temp
    gf temp_result[GF_POLY_LEN] = {0};

    /* init: set g(X) = (X - a_0) */
    t_gx[0] = gf_set[0];
    t_gx[1] = 1;

    /* compute (X - a_{t-1}) * ... * (X - a_1) * (X - a_0) */
    for (int i = 1; i < PARAM_T; i++)
    {
        /* multiplicate g(X) with (X - a_i) */
        for (int j = 0; j <= i; j++)
        {
            temp_result[j + 1] = t_gx[j];                                      // t(x) = g(x) * x
            temp_result[j] ^= gf2m_mul_w_tab(t_gx[j], gf_set[i], gf2m_tables); // t(x) = g(x) * x + g(x) * a_i = g(x) * (x - a_i)
            t_gx[j] = temp_result[j];                                          // new g(x) = t(x)
        }

        /* final compute */
        t_gx[i + 1] = temp_result[i + 1];

        /* set zero */
        temp_result[0] = 0;
    }

    /* return */
    for (int i = 0; i < PARAM_T; i++)
        gx[i] = t_gx[i];
}
