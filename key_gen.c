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

#include "key_gen.h"

/**
 * @brief Generate Parity-Check Matrix H
 *
 * @param [out] H Parity-Check Matrix H
 * @param [in] gf_poly goppa polynomial
 * @param [in] support_set support set
 * @param [in] gf2m_tables GF(2^m) Arith Precomputation Table
 */
void gen_parity_check_mat(OUT Word *H, IN const gf *gf_poly, IN const gf *support_set, IN const gf2m_tab *gf2m_tables)
{
    /* Generate ABC (\in F_{2^{13}[t][n] ) */
    gf mat_BC[PARAM_T][PARAM_N] = {0};
    gf mat_B[PARAM_N] = {0};
    gf mat_ABC[PARAM_T][PARAM_N] = {0};
    gf tmp_gf = 0;
    gf tmp_gf_poly[GF_POLY_LEN];

    /* Convert 'gf_poly(degree : t-1)' -> monic gx 'tmp_gf_poly(degree : t)' <-  */
    memcpy(tmp_gf_poly, gf_poly, sizeof(gf) * PARAM_T);
    tmp_gf_poly[PARAM_T] = 1;

    /* Generate C-Matrix */
    for (int i = 0; i < PARAM_N; i++)
    {
        gf galpha = gf_poly_eval(tmp_gf_poly, support_set[i], gf2m_tables);
        mat_BC[0][i] = gf2m_inv_w_tab(galpha, gf2m_tables->inv_tab);
    }

    /* Generate BC-Matrix */
    for (int i = 1; i < PARAM_T; i++)
    {
        for (int j = 0; j < PARAM_N; j++)
        {
            mat_BC[i][j] = gf2m_mul_w_tab(mat_BC[i - 1][j], support_set[j], gf2m_tables);
        }
    }

    /* generate ABC-Matrix */
    gf tmp;
    for (int i = 0; i < PARAM_T; i++) // A-Matrix row
    {
        for (int j = 0; j < PARAM_N; j++) // BC-Matrix col
        {
            tmp = 0;
            for (int k = 0; k < PARAM_T - i; k++) // A-Matrix col
            {
                tmp ^= gf2m_mul_w_tab(tmp_gf_poly[k + i + 1], mat_BC[k][j], gf2m_tables); // ABC-Matrix
            }
            mat_ABC[i][j] = tmp;
        }
    }

    /* Matrix ABC -> WORD_BITS-bit(Word)  */
    int row;
    Word tmp_Word;

    u8 cnt_remainder_Word = 0;
    u8 idx_remainder_Word = WORD_BITS - PARAM_M;

    for (int j = 0; j < PARAM_N; j++)
    {
        row = 0;
        for (int i = 0; i < PARAM_T; i++)
        {
            if (cnt_remainder_Word < idx_remainder_Word)
            {
                H[row + j * PK_NROWS_WORDS] |= (((Word)mat_ABC[i][j]) << cnt_remainder_Word);
                cnt_remainder_Word += PARAM_M;
            }
            else
            {
                tmp_Word = mat_ABC[i][j] & ((1 << (WORD_BITS - cnt_remainder_Word)) - 1);
                H[row + j * PK_NROWS_WORDS] |= (tmp_Word << cnt_remainder_Word);
                H[row + j * PK_NROWS_WORDS + 1] |= (mat_ABC[i][j] >> (WORD_BITS - cnt_remainder_Word));
                row++;
                cnt_remainder_Word -= idx_remainder_Word;
            }
        }
    }
}

/**
 * @brief Generate random Goppa code
 *
 * @param [out] support_set support set
 * @param [out] goppa_poly goppa polynomial
 * @param [out] H Parity-check Matrix H
 * @param [in] gf2m_tables GF(2^m) Arith Precomputation Table
 */
void gen_rand_goppa_code(OUT gf *support_set, OUT gf *goppa_poly, OUT Word *H, IN const gf2m_tab *gf2m_tables)
{
    gf gf_set[1 << PARAM_M] = {0};
    gf gf4goppapoly[PARAM_T] = {0};

    Word r_4_goppa_code[SEED_WORDS] = {0};

    gen_rand_sequence(r_4_goppa_code, SEED_BITS);

    /* Shuffle */
    shuffle(gf_set, (1 << PARAM_M), r_4_goppa_code);

    /* Generate support set */
    memcpy(support_set, gf_set, ((sizeof(gf)) * PARAM_N)); // Top n elements used as supportset elements

    /* Generate goppa polynomial */
    memcpy(gf4goppapoly, gf_set + PARAM_N, ((sizeof(gf)) * PARAM_T)); // next t elements are used as elements for gopa poly

    gen_goppa_poly(goppa_poly, gf4goppapoly, gf2m_tables); // generate goppa poly

    /* Generate Parity Check Matrix */
    gen_parity_check_mat(H, goppa_poly, support_set, gf2m_tables);
}

/**
 * @brief Generate scrambled code
 *
 * @param [out] H_hat a systematic scrambled parity-check Matrix H_hat
 * @param [out] S_inv Secret Key(Private Key): invertible Matrix S^{-1}
 * @param [out] r_perm_mat random bits r for generate random permutation Matrix P
 * @param [in] H a Parity-check Matrix H
 */
void get_scrambled_code(OUT Word *H_hat, OUT Word *S_inv, OUT Word *r_perm_mat, IN const Word *H)
{
    int hello = 1;
    while (1)
    {
        /* Generate Random Permutation Matrix P */
        gf P[PARAM_N] = {0};
        gf P_inv[PARAM_N] = {0};

        gen_rand_sequence(r_perm_mat, SEED_BITS);

        gen_rand_perm_mat(P, P_inv, PARAM_N, r_perm_mat);

        /* Generate a systematic scrambled parity-check Matrix H_hat */
        /* [ H_hat | S ] = [ HP | I_{n-k} ] */

        /* Generate Matrix H*P */
        Word HP[HP_WORDS] = {0};
        for (int i = 0; i < PARAM_N; i++)
            memcpy(HP + (HP_NROWS_WORDS * P[i]), H + (HP_NROWS_WORDS * i), sizeof(Word) * HP_NROWS_WORDS);

        /* S^{-1} : (n-k) x (n-k) */
        memcpy(S_inv, HP, (sizeof(Word) * S_INV_WORDS));

        int check = gaussian_row(HP, HP);

        if (check == 0)
        {
            memcpy(H_hat, HP, (sizeof(Word) * HP_WORDS));
            break;
        }
    }
}

/**
 * @brief Generate key pair
 *
 * @param [out] H_hat a systematic scrambled parity-check Matrix H_hat
 * @param [out] S_inv Secret Key(Private Key): invertible Matrix S^{-1}
 * @param [out] r_perm_mat random bits r for generate random permutation Matrix P
 * @param [in] H a Parity-check Matrix H
 */
void gen_key_pair(OUT PublicKey *pk, OUT SecretKey *sk, IN const gf2m_tab *gf2m_tables)
{
    memset(sk, 0, (size_t)SECRETKEYBYTES);
    Word H[HP_WORDS] = {0};

#if BENCH_MODE == ON
    uint64_t cyc_s;
    uint64_t cyc_e;
    cyc_s = cycle_read();
#endif

    gen_rand_goppa_code(sk->L, sk->gX, H, gf2m_tables);

#if BENCH_MODE == ON
    cyc_e = cycle_read();
    paloma_cycle_result.gen_rand_goppa_code = cyc_e - cyc_s;
#endif

#if BENCH_MODE == ON
    cyc_s = cycle_read();
#endif

    get_scrambled_code(H, sk->S_inv, sk->r, H);

#if BENCH_MODE == ON
    cyc_e = cycle_read();
    paloma_cycle_result.get_scrambled_code = cyc_e - cyc_s;
#endif

    /* pk <- H_hat[n-k:n] (is the sub matrix of H_hat consisting of the last k columns) */
    for (int i = 0; i < PK_WORDS; i++)
    {
        pk->H[i] = H[HP_NK_WORDS + i];
    }
}

/**
 * @brief Generation of a Random Permutation Matrix
 * @param [out] perm_mat_P An n × n permutation matrix P
 * @param [out] perm_mat_P_inv An n × n permutation matrix P^{−1}
 * @param [in] row row s.t. output row x row matrix (row == n)
 * @param [in] r A random 256-bit string r
 */
void gen_rand_perm_mat(OUT gf *perm_mat_P, OUT gf *perm_mat_P_inv, IN int row, IN const Word *r)
{
    // Shuffle([n], r)
    shuffle(perm_mat_P, row, r);

    for (size_t i = 0; i < row; i++)
        perm_mat_P_inv[perm_mat_P[i]] = i;
}
