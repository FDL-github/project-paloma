
#include "key_gen.h"
#include "goppa_instance.h"
#include "gf_poly.h"
#include <stdio.h>
#include "mat_mul.h"
#include "common.h"
#include <stdlib.h>
#include <time.h>

/**
* @brief Generate Parity-Check Matrix H
* @param [out] H Parity-Check Matrix H
* @param [in] gf_poly goppa polynomial
* @param [in] support_set support set
* @param [in] gf2m_tables GF(2^m) Arith Precomputation Table
*/
void gen_parity_check_mat(OUT u64* H, IN gf* gf_poly, IN gf* support_set, IN gf2m_tab* gf2m_tables)
{
    /* ---------------------------------------------------------------------- */
    /* Generate ABC (\in F_{2^{13}[t][n] ) */
    /* ---------------------------------------------------------------------- */
    gf mat_BC[PARAM_T][PARAM_N] = {0, };
    gf mat_B[PARAM_N] = {0, };
    gf mat_ABC[PARAM_T][PARAM_N] = {0, };
    gf tmp_gf = 0;

    /* C행렬 생성 */
    for(int i = 0; i < PARAM_N; i++)
    {
        gf galpha = gf_poly_eval(gf_poly, support_set[i], gf2m_tables);  
        mat_BC[0][i] = gf2m_inv_w_tab(galpha, gf2m_tables->inv_tab);          // C행렬 (g(a)^-1) 미리 곱해두기
    }
    
    /* BC행렬 생성 */
    for(int i = 1; i < PARAM_T; i++)
    {
        for(int j = 0; j < PARAM_N; j++)
        {
            mat_BC[i][j] = gf2m_mul_w_tab(mat_BC[i - 1][j], support_set[j], gf2m_tables);  // BC행렬
        }
    }

    /* ABC행렬 생성 */
    gf tmp;
    for(int i = 0; i < PARAM_T; i++)
    {    // A행렬 행  
        for(int j = 0; j < PARAM_N; j++)
        {      //BC 행렬 열
            tmp = 0;
            for(int k = 0; k < PARAM_T - i; k++)
            {      //A행렬 열
                tmp ^= gf2m_mul_w_tab(gf_poly[k + i + 1], mat_BC[k][j], gf2m_tables);        //ABC 행렬
            }
            mat_ABC[i][j] = tmp;    // gfPCMat 중복 확인 //확인
        }
    }

    /* ---------------------------------------------------------------------- */
    /* Matrix ABC -> 64-bit(u64) 빈 공간없이 저장 */
    /* ---------------------------------------------------------------------- */
    int row;
    u64 tmp_64;

    u8 cnt_remainder_u64 = 0;
    u8 idx_remainder_u64 = 64 - Param_M;    // 51 = 64 - 13

    for(int j = 0; j < PARAM_N; j++)
    {
        row = 0;
        for(int i = 0; i < PARAM_T; i++)
        {
            if(cnt_remainder_u64 < idx_remainder_u64)   /* u64에 남은 공간이 13비트 이상인 경우 */
            {
                H[row + j * (Param_M * PARAM_T / 64)] |= (((u64)mat_ABC[i][j]) << cnt_remainder_u64);
                cnt_remainder_u64 += Param_M;
            }
            else    /* u64에 남은 공간이 13비트 미만인 경우 */
            {   
                tmp_64 = mat_ABC[i][j] & ((1 << (64 - cnt_remainder_u64)) - 1);
                H[row + j * (Param_M * PARAM_T / 64)] |= (tmp_64 << cnt_remainder_u64);
                H[row + j * (Param_M * PARAM_T / 64) + 1] |= (mat_ABC[i][j] >> (64 - cnt_remainder_u64)) ;
                row++;
                cnt_remainder_u64 -= idx_remainder_u64;                
            }
        }
    }
}

/**
* @brief Generate Random Goppa code
* @param [out] support_set support set
* @param [out] goppa_poly goppa polynomial
* @param [out] H Parity-check Matrix H
* @param [in] gf2m_tables GF(2^m) Arith Precomputation Table
*/
void gen_rand_goppa_code(OUT gf* support_set, OUT gf* goppa_poly, OUT u64* H, IN gf2m_tab* gf2m_tables)
{
    gf gf_set[1 << Param_M]={0,}; 
    // gf gf_goppa_poly[n]={0,};
    gf gf_goppa_poly[PARAM_T + 1]={0,};

    /* 유한체 gf 원소 n + t개 선택 */   
    // 256
    u64 r_4_goppa_code[4];

    gen_rand_sequence(r_4_goppa_code, 256);

    // shuffle
    shuffle(gf_set, (1 << Param_M), r_4_goppa_code);

    /* Generate support set */ 
    memcpy(support_set, gf_set, ((sizeof(gf)) * PARAM_N));    // 상위 n개의 원소는 supportset 원소로 사용

    /* Generate goppa polynomial */
    memcpy(gf_goppa_poly, gf_set + PARAM_N, ((sizeof(gf)) * (PARAM_T + 1)));   // 그 다음 t개의 원소는 goppa poly용 원소로 사용

    gen_goppa_poly(goppa_poly, gf_goppa_poly, gf2m_tables);  // goppa poly 생성
    
    /* Generate Parity Check Matrix */
    gen_parity_check_mat(H, goppa_poly, support_set, gf2m_tables);
}

/**
* @brief Generation of a Scrambled Code
* @param [out] H_hat a systematic scrambled parity-check matrix H_hat
* @param [out] S_inv Secret Key(Private Key): invertible matrix S^{-1}
* @param [out] r_perm_mat random bits r for generate random permutation matrix P 
* @param [in] H a Parity-check matrix H
*/
void get_scrambled_code(OUT u64* H_hat, OUT u64* S_inv, OUT u64* r_perm_mat, IN u64* H)
{   
    while(1)
    {
        /* Generate Random Permutation Matrix P */
        gf P[PARAM_N] = {0, };    // 0 or 1로 구성된 n x n matrix?
        
        gen_rand_sequence(r_perm_mat, 256);

        gen_rand_perm_mat(P, PARAM_N, r_perm_mat);

        /* Generate a systematic scrambled parity-check matrix H_hat */
        /* [ H_hat | S ] = [ HP | I_{n-k} ] */
        
        /* Generate Matrix H*P */
        u64 HP[Param_M * (PARAM_T / 64) * PARAM_N] = {0, };
        memcpy(HP, H, (sizeof(u64)) * (Param_M * PARAM_T * PARAM_N / 64));

        u64 tmp = 0;
        for(int i = 0; i < PARAM_N; i++)
        {
            for(int j = 0; j < Param_M * (PARAM_T / 64); j++)
            {   
                /* Multiplication Perumutation Mat P: swap index */ 
                tmp = HP[j + (P[i] * Param_M * (PARAM_T / 64))];  
                HP[j + (P[i] * Param_M * (PARAM_T / 64))] = HP[j + (i * Param_M * (PARAM_T / 64))]; 
                HP[j + (i * Param_M * (PARAM_T / 64))] = tmp;
            }
        }

        // S^{-1} : (n-k) x (n-k)
        memcpy(S_inv, HP, (sizeof(u64) * (Param_M * PARAM_T * Param_M * PARAM_T / 64)));

        int check = gaussian_row(HP, HP);
        
        /* full rank가 될때까지 위의 과정 반복 */
        if(check == 0)
        {   
            // H_hat : (n-k) x n,   S : (n-k) x (n-k)
            memcpy(H_hat, HP, (sizeof(u64) * (Param_M * PARAM_T * PARAM_N / 64)));
     
            break;
        }
        // goto while(1) : if H_hat_[n-k] is not Identity Matrix (if not systematic)
    }
}

/**
* @brief Generation of Key Pair
* @param [out] pk a public key pk = M (= H_hat[n-k:n])
* @param [out] sk a secret key sk = (L, g(X), S^{-1}, r) s.t. r is a seed of Perm Mat P.
*/
void gen_key_pair(OUT u64* pk, OUT u64_sk* sk, IN gf2m_tab* gf2m_tables)
{
    /* Generate Goppa Code */
    gf L[PARAM_N] = {0, }; // Support Set L
    gf g_X[PARAM_T + 1] = {0, }; // goppa polynomial g(X)
    u64 H[(Param_M * PARAM_T) * PARAM_N / 64] = {0, };
    
    gen_rand_goppa_code(L, g_X, H, gf2m_tables);

    /* Get Scrambled Code */
    u64 S_inv[Param_M * PARAM_T * Param_M * PARAM_T / 64] = {0, };
    u64 r4perm[4] = {0, }; // 4 = 256-bit / 64-bit(= u64)

    get_scrambled_code(H, S_inv, r4perm, H);

    /* sk <- (L, g(X), S^{-1}, r) */
    int num = 0;
    int cnt_remainder_u64 = 0;
    int idx_remainder_u64 = 64 - Param_M;    // 51 = 64 - 13
    u64 tmp;

    memset(sk, 0, (size_t)SECRETKEYBYTES);
    
    /* Support Set L */
    for(int i = 0; i < PARAM_N; i++)
    {
        if(cnt_remainder_u64 < idx_remainder_u64)
        {
            sk->L[num] |= (((u64)L[i]) << (cnt_remainder_u64));
            cnt_remainder_u64 += Param_M;
        }
        else
        {
            tmp = L[i] & ((1 << (64 - cnt_remainder_u64)) - 1);
            sk->L[num] |= tmp << (cnt_remainder_u64);
            sk->L[num + 1] |= (L[i] >> (64 - cnt_remainder_u64));
            num ++;
            cnt_remainder_u64 -= idx_remainder_u64;
        }
    }
  
    /* Goppa Polynomial g(X) */
    for(int i = 0; i <= PARAM_T; i++)
    {
        if(cnt_remainder_u64 < idx_remainder_u64){
            sk->g_X[num] |= (((u64)g_X[i]) << (cnt_remainder_u64));
            cnt_remainder_u64 += Param_M;
        }
        else
        {
            u64 temp = g_X[i] & ((1 << (64 - cnt_remainder_u64)) - 1);
            sk->g_X[num] |= temp << (cnt_remainder_u64);
            sk->g_X[num + 1] |= (g_X[i] >> (64 - cnt_remainder_u64));
            num ++;
            cnt_remainder_u64 -= idx_remainder_u64;
        }
    }

    /* S_inv : (n - k) * (n - k) = mt * mt */
    memcpy(sk->S_inv, S_inv, (sizeof(u64)) * (Param_M * PARAM_T) * (Param_M * PARAM_T) / 64);

    /* r : 256-bit string*/
    memcpy(sk->r4perm, r4perm, (sizeof(u64) * 4));
    
    /* pk <- H_hat[n-k:n] (is the submatrix of H_hat consisting of the last k columns) */
    // memcpy(pk, H_hat, (sizeof(u64)) * (Param_M * n * t / 64));재
    tmp = (Param_M * PARAM_T * (PARAM_N - Param_M * PARAM_T) / 64);
    for(int i = 0; i < tmp; i++){
        pk[i] = H[(Param_M * PARAM_T * Param_M * PARAM_T) / 64 + i];
    }    
}

/**
 * @brief Generation of a Random Permutation Matrix
 * @param [out] perm_mat_P An n × n permutation matrix P, P−1
 * @param [in] row row s.t. output row x row matrix (row == n)
 * @param [in] r A random 256-bit string r
 */
void gen_rand_perm_mat(OUT gf* perm_mat_P, IN int row, IN u64* r)
{   
    // Shuffle([n], r)
    shuffle(perm_mat_P, row, r);
}
