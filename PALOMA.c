 #include "api.h"
#include <stdio.h>
#include "key_gen.h"
#include "encap.h"
// #include "decap.h"
#include "config.h"

int crypto_kem_keypair(unsigned char* pk, unsigned char* sk, gf2m_tab* gf2m_tables)
{
    u64 pk64[PUBLICKEYBYTES / 8] = {0, };
    u64_sk sk64;

    gen_key_pair(pk64, &sk64, gf2m_tables);

    for(int i = 0; i< (PUBLICKEYBYTES / 8); i++)
    {
        for(int j = 0; j < 8; j++)
        {
            pk[8 * i + j] = (pk64[i] >> (8 * j)) & 0xff;
        }
    }
    
    int idx = 0;
    for(int i = idx; i < (PARAM_N * Param_M / 64); i++)
    {
        for(int j = 0; j < 8; j++)
        {
            sk[8 * i + j] = ((sk64.L[i]) >> (8 * j)) & 0xff;
        }
    }

    idx += (PARAM_N * Param_M / 64);
    for(int i = idx; i < (idx + ((PARAM_T + 1) * Param_M / 64)); i++)
    {
        for(int j = 0; j < 8; j++)
        {
            sk[8 * i + j] = ((sk64.g_X[i]) >> (8 * j)) & 0xff;
        }
    }

    idx += ((PARAM_T + 1) * Param_M / 64);
    for(int i = idx; i < (idx + (Param_M * PARAM_T * Param_M * PARAM_T / 64)); i++)
    {
        for(int j = 0; j < 8; j++)
        {
            sk[8 * i + j] = ((sk64.S_inv[i]) >> (8 * j)) & 0xff;
        }
    }

    idx += (Param_M * PARAM_T * Param_M * PARAM_T / 64);
    for(int i = idx; i < (idx + 4); i++)
    {
        for(int j = 0; j < 8; j++)
        {
            sk[8 * i + j] = ((sk64.r4perm[i]) >> (8 * j)) & 0xff;
        }
    }
    
    return 0;
}