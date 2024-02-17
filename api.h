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

#ifndef API_H
#define API_H

#include "config.h"

#define CRYPTO_PUBLICKEYBYTES PUBLICKEYBYTES
#define CRYPTO_SECRETKEYBYTES SECRETKEYBYTES
#define CRYPTO_CIPHERTEXTBYTES CIPHERTEXTBYTES
#define CRYPTO_BYTES BYTES

/* Change the algorithm name */
#define CRYPTO_ALGNAME "PALOMA"

#include "gf.h"
#include "gf_tab_gen.h"

int crypto_kem_keypair(unsigned char *pk, unsigned char *sk, const gf2m_tab *gf2m_tables);
int crypto_kem_enc(unsigned char *ct, unsigned char *key, const unsigned char *pk);
int crypto_kem_dec(unsigned char *key, const unsigned char *ct, const unsigned char *sk, const gf2m_tab *gf2m_tables);

#endif /* api_h */