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

#include "api.h"
#include "key_gen.h"
#include "encap.h"
#include "decap.h"
#include "gf_tab_gen.h"

void print_bytes(const unsigned char *bytes, int size)
{
    for (int i = 0; i < size / 16; i++)
    {
        for (int j = 0; j < 16; j++)
        {
            printf("%02x ", bytes[16 * i + j]);
        }

        puts("");
    }
}

/* 인캡, 디캡 잘 되는지 획인하는 코드. */
void paloma_valid_check()
{
    int invalid = 0;

    unsigned char ct[CIPHERTEXTBYTES] = {0},
                  key[CRYPTO_BYTES] = {0},
                  rec_key[CRYPTO_BYTES] = {0},
                  pk[PUBLICKEYBYTES] = {0},
                  sk[SECRETKEYBYTES] = {0};

    gf2m_tab gf2m_tables;
    gen_precomputation_tab(&gf2m_tables);

    /* kem */
    crypto_kem_keypair(pk, sk, &gf2m_tables);
    crypto_kem_enc(ct, key, pk);
    crypto_kem_dec(rec_key, ct, sk, &gf2m_tables);

    /* valid check */
    for (int i = 0; i < sizeof(key); i++)
    {
        if (rec_key[i] != key[i])
            invalid = 1;
    }

    /* result */
    puts("========== Is key well recovered? ==========");

    puts("original key:");
    print_bytes(key, CRYPTO_BYTES);

    puts("recovered key:");
    print_bytes(rec_key, CRYPTO_BYTES);

    if (invalid)
        puts("\nfail.");
    else
        puts("\nsuccess.");

    puts("============================================\n");
}

int main()
{
    srand(time(NULL));

    for (int i = 0; i < 10; i++)
    {
        paloma_valid_check();
    }

    return 0;
}
