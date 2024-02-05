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
    // for(int i = 0; i < (PARAM_N - PARAM_K) / 64; i++)
    // {
    //     synd[i]=e[i];                   // [I]e 계산.
    // }   
    memcpy(syndrome, e, (sizeof(u64)) * ((PARAM_N - PARAM_K) / 64));

    // u64 err_k[MAXN/64]={0,}; 
    // for(int i=0;i<k/64;i++) err_k[i] = e[(n-k)/64+i];   //e의 하위 k비트 선택

    for(int i = 0; i < PARAM_K / 64; i++) 
    {
        // err_k[i] = e[(PARAM_N - PARAM_K) / 64 + i];   //e의 하위 k비트 선택
        // 이렇게 변경해도 되나?
        e[i] = e[(PARAM_N - PARAM_K) / 64 + i];   //e의 하위 k비트 선택
    }

    u64 tmp[MAXN/64]={0,};
    /* [M] X e_j */
    matXvec(tmp, pk, e,(PARAM_N - PARAM_K), PARAM_K);       // [M] e_j 계산.    

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

    /* 해밍무게가 t인 랜덤 벡터 생성 */
    u64 ep[MAXN/64]={0,};
    u64 e_hat[MAXN/64]={0,};

    gen_rand_err_vec(ep, r4errvec);

    // /* r = ROG(ep) */
    // int check = RandOracle(r, ep, n, 1);

    // /* 생성한 난수로 임의의 P 생성. */
    // gf permmatP[MAXN] = {0,};

    // GenRandPermMat(permmatP, n, r);

    // /* ep 치환 */ 
    // for(int i=0;i<n/64;i++) e_hat[i] = ep[i];

    // u64 One = 1;
    // for(int i=n-1;i>=0;i--){
    //     if(((e_hat[i/64] >> (i%64))&1) != ((e_hat[permmatP[i]/64]>>(permmatP[i]%64))&1)){       // 두 비트가 다른 경우에만 ^1 해서 바꾸기. 
    //         e_hat[i/64] ^= One<<(i%64);
    //         e_hat[permmatP[i]/64] ^= (One<<(permmatP[i]%64));         //check
    //     }
    // }

    // /* encrypt */
    // Encrypt(s_hat, pk, e_hat,n,k);

    // /* k = ROH(ep, rhat, shat) */
    // u64 input[MAXN/64 + 4 + (13*MAXT)/64 ]={0,};

    // for(int i=0;i<n/64;i++){
    //     input[i] = ep[i];
    // }
    // for(int i=0; i<4;i++){
    //     input[i+(n/64)] = r[i];
    // }
    // for(int i=0; i<(n-k)/64;i++){
    //     input[i+4+(n/64)] = s_hat[i];
    // }

    // int check2 = RandOracle(key, input, n + 256 + (n-k), 2);

}
