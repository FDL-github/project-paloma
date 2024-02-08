#include "encap.h"
#include "mat_mul.h"
#include "common.h"
#include "key_gen.h"
#include <stdio.h>
#include <string.h>

/**
* @brief encrypt
* @param [out] synd 신드롬
* @param [in] pk : 공개키
* @param [in] e : 오류벡터
*/
void encrypt(OUT u64* syndrome, IN u64* pk, IN u64* e)
{
    /* 항등행렬 부분 행렬곱 */ 
    memcpy(syndrome, e, (sizeof(u64)) * ((PARAM_N - PARAM_K) / 64));

    for(int i = 0; i < PARAM_K / 64; i++) 
    {
        e[i] = e[(PARAM_N - PARAM_K) / 64 + i];   //e의 하위 k비트 선택
    }

    u64 tmp[PARAM_N / 64] = {0, };
    /* [M] X e_j */
    // matXvec(tmp, pk, err_k,(PARAM_N - PARAM_K), PARAM_K);       // [M] e_j 계산.    
    matXvec(tmp, pk, e, (PARAM_N - PARAM_K), PARAM_K);       // [M] e_j 계산.    
    
    /* [I|M] X e_j */
    for(int i = 0; i < (PARAM_N - PARAM_K) / 64; i++)
    {
        syndrome[i] ^= tmp[i];                   // [I|M]e 계산.
    }   
}

/**
* @brief weight가 t이고 길이가 n인 오류벡터 생성
* @param [out] error_vector 오류 벡터
* @param [in] seed 오류벡터 생성을 위한 시드
*/
void gen_rand_err_vec(OUT u64* error_vector, IN u64* seed)
{
    /* seed 이용 길이 n 배열 셔플 */
    gf err[PARAM_N] = {0, };
    u64 One = 1;

    shuffle(err, PARAM_N, seed);

    for(int i = 0; i < PARAM_T; i++)
    {
        error_vector[err[i] / 64] |= (One << (err[i] % 64));
    }
}

/**
* @brief Encapsulation
* @param [out] key 256-bit key
* @param [out] r_hat 256-bit string (c = (r_hat, s_hat))
* @param [out] s_hat syndrome (c = (r_hat, s_hat))
* @param [in] pk a public key pk
*/
void encap(OUT u64* key, OUT u64* r_hat, OUT u64* s_hat, IN u64* pk)
{
    /* 256비트 난수 생성 */
    u64 r4errvec[4] = {0, };    
    gen_rand_sequence(r4errvec, 256);

    // for (size_t i = 0; i < 4; i++)
    // {
    //     printf("%x ", r4errvec[i]);
    // }
    
    /* 해밍무게가 t인 랜덤 벡터 생성 */
    u64 ep[PARAM_N / 64] = {0, };
    u64 e_hat[PARAM_N / 64] = {0, };

    gen_rand_err_vec(ep, r4errvec);
    
    // for (size_t i = 0; i < PARAM_N / 64; i++)
    // {
    //     printf("%x ", ep[i]);
    // }
    
    /* r = ROG(ep) */
    int check = rand_oracle(r_hat, ep, PARAM_N, 1);

    // for (size_t i = 0; i < 4; i++)
    // {
    //     printf("%x ", r_hat[i]);
    // }

    /* 생성한 난수로 임의의 P 생성. */
    gf perm_mat_P[PARAM_N] = {0,};

    gen_rand_perm_mat(perm_mat_P, PARAM_N, r_hat);

    // for (size_t i = 0; i < PARAM_N; i++)
    // {
    //     printf("%x ", perm_mat_P[i]);
    // }

    /* ep 치환 */ 
    memcpy(e_hat, ep, (sizeof(u64)) * PARAM_N / 64);
    // for(int i=0;i<n/64;i++) e_hat[i] = ep[i];

    u64 One = 1;
    for(int i = PARAM_N - 1; i >= 0; i--)
    {
        if(((e_hat[i / 64] >> (i % 64)) & 1) != ((e_hat[perm_mat_P[i]/64]>>(perm_mat_P[i]%64))&1))
        {   // 두 비트가 다른 경우에만 ^1 해서 바꾸기. 
            e_hat[i/64] ^= One<<(i%64);
            e_hat[perm_mat_P[i]/64] ^= (One<<(perm_mat_P[i]%64));         //check
        }
    }

    // for (size_t i = 0; i < (PARAM_N/64); i++)
    // {
    //     printf("%x ", e_hat[i]);
    // }

    /* encrypt */
    encrypt(s_hat, pk, e_hat);

    // for (size_t i = 0; i < (Param_M*PARAM_T/64); i++)
    // {
    //     printf("%x ", s_hat[i]);
    // }

    /* k = ROH(ep, rhat, shat) */
    u64 input[(PARAM_N / 64) + 4 + (13 * PARAM_T)/64] = {0, };

    // memcpy(input, ep, (sizeof(u64)) * PARAM_N / 64);

    // memcpy(input + ((sizeof(u64)) * PARAM_N / 64), r4errvec, (sizeof(u64)) * 4);

    // memcpy(input + ((sizeof(u64)) * PARAM_N / 64) + 4, s_hat, (sizeof(u64)) * (PARAM_N - PARAM_K)/64);

    for(int i=0;i<PARAM_N/64;i++){
        input[i] = ep[i];
    }
    for(int i=0; i<4;i++){
        input[i+(PARAM_N/64)] = r_hat[i];
    }
    for(int i=0; i<(PARAM_N-PARAM_K)/64;i++){
        input[i+4+(PARAM_N/64)] = s_hat[i];
    }

    int check2 = rand_oracle(key, input, PARAM_N + 256 + (PARAM_N - PARAM_K), 2);

}
