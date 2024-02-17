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

/*
안녕..
*/

#ifndef DECODING_H_
#define DECODING_H_

#include "config.h"
#include "gf.h"
#include "gf_poly.h"
#include "mat_mul.h"
#include "common.h"
#include "key_gen.h"
#include "encap.h"
#include "bench.h"

void to_poly(OUT gf *poly, IN const Word *vec, IN int deg);
void construct_key_equ(OUT gf *vX, OUT gf *g1X, OUT gf *g2X, OUT gf *g12X, IN const gf *sX, IN const gf *gX, IN const gf2m_tab *gf2m_tables);
void solve_key_equ(OUT gf *a1X, OUT gf *b2X, IN const gf *vX, IN const gf *g12X, IN int deg_a, IN int deg_b, IN const gf2m_tab *gf2m_tables);
void get_err_loc_poly(OUT gf *sigX, IN const gf *a2X, IN const gf *g2X, IN const gf *b2X, IN const gf *g1X, IN const gf2m_tab *gf2m_tables);
void find_err_vec(OUT Word *err_vec, IN const gf *sigX, IN const gf *L, IN const gf2m_tab *gf2m_tables);

void rec_err_vec(OUT Word *err_vec, IN const SecretKey *sk, IN const Word *synd_vec, IN const gf2m_tab *gf2m_tables);

#endif