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

#ifndef KEY_GEN_H
#define KEY_GEN_H

#include "config.h"
#include "gf.h"
#include "gf_poly.h"
#include "goppa_instance.h"
#include "mat_mul.h"
#include "common.h"
#include "bench.h"

typedef struct
{
    gf L[PARAM_N];
    gf gX[PARAM_T];
    Word S_inv[S_INV_WORDS];
    Word r[SEED_WORDS];
} SecretKey;

typedef struct
{
    Word H[PK_WORDS];
} PublicKey;

void gen_key_pair(OUT PublicKey *pk, OUT SecretKey *sk, IN const gf2m_tab *gf2m_tables);

void gen_rand_goppa_code(OUT gf *support_set, OUT gf *goppa_poly, OUT Word *H, IN const gf2m_tab *gf2m_tables);
void get_scrambled_code(OUT Word *H_hat, OUT Word *S_inv, OUT Word *rand4P, IN const Word *H);
void gen_rand_perm_mat(OUT gf *perm_mat_P, OUT gf *perm_mat_P_inv, IN int row, IN const Word *r);

void gen_parity_check_mat(OUT Word *H, IN const gf *gf_poly, IN const gf *support_set, IN const gf2m_tab *gf2m_tables);

#endif