 #include "api.h"
#include <stdio.h>
#include "key_gen.h"
#include "encap.h"
// #include "decap.h"
#include "config.h"

/**
 * @brief Generates public and secret(private) key
 * 
 * @param pk pointer to output public key
 *           (a structure composed of (matrix H[n-k]))
 * @param sk pointer to output secret key 
 *           (a structure composed of (support set L, goppa polynomial g(X), matrix S^{-1}, seed r for permutation matrix)
 * @param gf2m_tables precomputation table of GF(2^m)
 *                    (multiplication, square, square root, inverse)
 * @return int
 */
int crypto_kem_keypair(unsigned char* pk, unsigned char* sk, gf2m_tab* gf2m_tables)
{
    u64_pk pk64;
    u64_sk sk64;

    gen_key_pair(&pk64, &sk64, gf2m_tables);

    /* Public Key */
    for(int i = 0; i< (PUBLICKEYBYTES / 8); i++)
    {
        for(int j = 0; j < 8; j++)
        {
            pk[8 * i + j] = ((&pk64)->H[i] >> (8 * j)) & 0xff;
        }
    }
    
    /* Secret Key - Support set L */
    int idx = 0;
    for(int i = idx; i < (PARAM_N * Param_M / 64); i++)
    {
        for(int j = 0; j < 8; j++)
        {
            sk[8 * i + j] = (((&sk64)->L[i - idx]) >> (8 * j)) & 0xff;
        }
    }

    /* Secret Key - Goppa polynomial g(X) */
    idx += (PARAM_N * Param_M / 64);
    for(int i = idx; i < (idx + ((PARAM_T + 1) * Param_M / 64)); i++)
    {
        for(int j = 0; j < 8; j++)
        {
            sk[8 * i + j] = (((&sk64)->g_X[i - idx]) >> (8 * j)) & 0xff;
        }
    }

    /* Secret Key - matrix S^{-1} */
    idx += ((PARAM_T + 1) * Param_M / 64);
    for(int i = idx; i < (idx + (Param_M * PARAM_T * Param_M * PARAM_T / 64)); i++)
    {
        for(int j = 0; j < 8; j++)
        {
            sk[8 * i + j] = (((&sk64)->S_inv[i - idx]) >> (8 * j)) & 0xff;
        }
    }

    /* Secret Key - seed r for permutation matrix */
    idx += (Param_M * PARAM_T * Param_M * PARAM_T / 64);
    for(int i = idx; i < (idx + 4); i++)
    {
        for(int j = 0; j < 8; j++)
        {
            sk[8 * i + j] = ((((&sk64)->r4perm[i - idx]) >> (8 * j)) & 0xff);
        }
    }

    return 0;
}

int crypto_kem_enc(unsigned char* ct, unsigned char* key, const unsigned char* pk)
{
    u64 key64[4]={0,};
    u64 r[4]={0,};
    u64 shat[Param_M*PARAM_T/64]={0,};

    
    u64 pk64[Param_M*PARAM_T*(PARAM_N-(Param_M*PARAM_T))/64]={0,};
    int tmp = Param_M*PARAM_T*(PARAM_N-(Param_M*PARAM_T))/64;
    
    for(int i=0; i<tmp; i++){
        for(int j=0;j<8;j++){
            pk64[i] |= (((u64)pk[8*i+j])<<(8*j));
        }
    }

    encap(key64,r,shat,pk64);

    for(int i=0; i<4; i++){
        for(int j=0;j<8;j++){
            ct[8*i+j] = (r[i]>>(8*j))&0xff;
        }
    }

    tmp =Param_M*PARAM_T/64;
    for(int i=0; i<tmp; i++){
        for(int j=0;j<8;j++){
            ct[32+8*i+j] = (shat[i]>>(8*j))&0xff;
        }
    }

    for(int i=0; i<4; i++){
        for(int j=0;j<8;j++){
            key[8*i+j] = (key64[i]>>(8*j))&0xff;
        }
    }
    return 0;
}