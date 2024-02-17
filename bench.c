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

#include "bench.h"

#if BENCH_MODE == ON

uint64_t cycle_start;
uint64_t cycle_end;
PalomaCycleResult paloma_cycle_result;

double time_start;
double time_end;
PalomaTimeResult paloma_time_result;

#endif

void paloma_check()
{
    puts("\n======================================== current settings");
    printf("PALOMA MODE(0:128, 1:192: 2:256): %d\n", PALOMA_MODE);
    printf("WORD(8, 32, 64): %d\n", WORD);
    printf("BENCH MODE(0:OFF, 1:ON): %d\n", BENCH_MODE);

    srand(time(NULL));

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
    puts("\n======================================== Is key well recovered?");

    puts("original key:");
    for (int i = 0; i < 32; i++)
        printf("%02X", key[i]);

    puts("");

    puts("recovered key:");
    for (int i = 0; i < 32; i++)
        printf("%02X", rec_key[i]);

    puts("");

    if (invalid)
        puts("\nfail.");
    else
        puts("\nsuccess.");
}

void paloma_cycle_print()
{
    printf("\n==================================================\n");
    printf("cycle: %llu \t [key generation]\n", paloma_cycle_result.gen_key);
    printf("cycle: %llu \t gen_rand_goppa_code\n", paloma_cycle_result.gen_rand_goppa_code);
    printf("cycle: %llu \t get_scrambled_code\n", paloma_cycle_result.get_scrambled_code);

    printf("\n==================================================\n");
    printf("cycle: %llu \t [encapsulation]\n", paloma_cycle_result.encap);
    printf("cycle: %llu \t encrypt\n", paloma_cycle_result.encrypt);

    printf("\n==================================================\n");
    printf("cycle: %llu \t [decapsulation]\n", paloma_cycle_result.decap);
    printf("cycle: %llu \t decrypt\n", paloma_cycle_result.decrypt);
    printf("cycle: %llu \t construct_key_equ\n", paloma_cycle_result.construct_key_equ);
    printf("cycle: %llu \t solve_key_equ\n", paloma_cycle_result.solve_key_equ);
    printf("cycle: %llu \t find_err_vec\n", paloma_cycle_result.find_err_vec);
}