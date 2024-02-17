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

#include "common.h"

/**
 * @brief PALOMA SHUFFLE : Shuffle with 256-bit seed r
 *
 * @param [out] shuffled_set
 * @param [in] set_len Set length.
 * @param [in] _256bits_seed_r 256-bit seed r
 */
void shuffle(OUT gf *shuffled_set, IN int set_len, IN const Word *_256bits_seed_r)
{
    int i, j;
    int w = 0;
    gf seed[16];
    gf tmp;

    /* Generate [0,1,...,n-1] set */
    for (i = 0; i < set_len; i++)
    {
        shuffled_set[i] = i;
    }

    /* Separate seeds by 16-bit */
    memcpy(seed, _256bits_seed_r, SEED_BYTES);

    /* Shuffling */
    for (i = set_len - 1; i > 0; i--)
    {
        j = ((seed[w % 16]) % (i + 1));
        /* Swap */
        tmp = shuffled_set[j];
        shuffled_set[j] = shuffled_set[i];
        shuffled_set[i] = tmp;

        w = (w + 1) & 0xf;
    }
}

/**
 * @brief Function to generate random sequence
 *
 * @param [out] out_vec Random sequence
 * @param [in] sequence_len Bit length of random sequence
 */
void gen_rand_sequence(OUT Word *rand_sequence, IN int sequence_len)
{
    unsigned char seed[1];

    for (int i = 0; i < (sequence_len / WORD_BITS); i++) // WORD_BITS : bit size of Word
    {
        rand_sequence[i] = 0;

        for (int j = 0; j < WORD_BYTES; j++) // WORD_BYTES : bit size of Word / WORD_BYTES
        {
            randombytes(seed, 1);
            Word rand_k = seed[0] & 0xff;
            rand_sequence[i] |= (rand_k << (WORD_BYTES * (WORD_BYTES - 1 - j)));
        }
    }
}

/**
 * @brief Random Oracle
 *
 * @param [in] msg: Oracle input data
 * @param [in] msg_len: Bit length of oracle input data
 * @param [in] oracle_num: 1 : oracle G, 2 : oracle H
 * @param [out] seed: Oracle result
 */
int rand_oracle(OUT Word *seed, IN const Word *msg, IN int msg_len, IN int oracle_num)
{
    /* Use LSH-512, output length : 512-bit */
    lsh_type algtype = LSH_MAKE_TYPE(1, 512);
    lsh_u8 src[WORD_BYTES + (msg_len / WORD_BYTES)];

    /* Generate oracle input data */
    /* PALOMAGG or PALOMAHH ASCII value*/
    src[0] = 0x50;
    src[1] = 0x41;
    src[2] = 0x4c;
    src[3] = 0x4f;
    src[4] = 0x4d;
    src[5] = 0x41;

    if (oracle_num == 1)
    {
        src[6] = 0x47;
        src[7] = 0x47;
    }
    else if (oracle_num == 2)
    {
        src[6] = 0x48;
        src[7] = 0x48;
    }
    else
    {
        return -1;
    }
    /* input data */
    for (int i = 0; i < (msg_len / WORD_BYTES); i++)
    {
        src[WORD_BYTES + i] = (msg[i / WORD_BYTES] >> ((WORD_BYTES * i) % WORD_BITS)) & 0xff;
    }

    /* Store output hash values */
    lsh_u8 result[512 / 8] = {0};

    /* Generate hash values */
    lsh_digest(algtype, src, WORD_BITS + msg_len, result);

    for (int i = 0; i < 32; i++)
    {
        seed[i / WORD_BYTES] |= ((Word)result[i] << ((i * WORD_BYTES) % WORD_BITS));
    }

    return 0;
}
