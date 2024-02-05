/*
 * Copyright (c) 2022 FDL(Future cryptography Design Lab.) Kookmin University
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
#include <stdlib.h>
#include <stdio.h>
//! #include "rng.h" //! 난수생성기 만들기 전에 잠시 봉인.
#include <time.h>

/**
 * @brief generate goppa polynomial (x-a_1)(x-a_2)...(x-a_t).
 *
 * @param gx goppa polynomial.
 * @param gf_set number t GF(2^m) elements.
 * @param gf2m_tables tables for efficient arithmetic over GF(2^m).
 */
void gen_goppa_poly(OUT gf *gx, IN gf *gf_set, IN gf2m_tab *gf2m_tables)
{
    gf t_gx[PARAM_T + 1] = {0};
    gf tx[PARAM_T + 1] = {0};

    // init: set g(x) = x - a_0
    t_gx[0] = gf_set[0];
    t_gx[1] = 1;

    // compute (x - a_t) * ... * (x - a_1) * (x - a_0)
    for (int i = 1; i < PARAM_T; i++)
    {
        // multiplicate g(x) with (x - a_i)
        for (int j = 0; j <= i; j++)
        {
            tx[j + 1] = t_gx[j];                                      // t(x) = g(x) * x
            tx[j] ^= gf2m_mul_w_tab(t_gx[j], gf_set[i], gf2m_tables); // t(x) = g(x) * x + g(x) * a_i = g(x) * (x - a_i)
            t_gx[j] = tx[j];                                          // new g(x) = t(x)
        }

        // final compute
        t_gx[i + 1] = tx[i + 1];

        // set zero
        tx[0] = 0;
    }

    // return
    for (int i = 0; i <= PARAM_T; i++)
        gx[i] = t_gx[i];
}

/**
 * @brief 유한체 F_2^m을 섞어 x개 만큼만 배열로 출력.
 */

/**
 * @brief generate set of random elements over F_2^m.
 *
 * This function use shuffle algorithm.
 *
 * @param sf_set set of random elements.
 * @param size number of random elements be generated.
 */
void gen_random_elements_of_gf2m(OUT gf *sf_set, IN int size)
{
    gf set[(1 << Param_M)];

    for (int i = 0; i < (1 << Param_M); i++)
        set[i] = i;

    for (int i = (1 << Param_M) - 1; i > 0; i--)
    {
        u8 seed[2];
        u32 rand_num;

        //! randombytes(seed, 2);
        // srand(time(NULL));
        seed[0] = (u8)rand();
        seed[1] = (u8)rand();
        //! 일단 rand 함수로 사용.

        rand_num = seed[0];
        rand_num ^= (seed[1] << 8);

        int j = rand_num % (i + 1);

        gf tmp = set[j];
        set[j] = set[i];
        set[i] = tmp;
    }

    // return
    for (int i = 0; i < size; i++)
        sf_set[i] = set[i];
}