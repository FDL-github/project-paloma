#ifndef KEY_GEN_H
#define KEY_GEN_H

#include "config.h"
#include "gf.h"

typedef struct{   
    u64 L[PARAM_N * Param_M / 64];
    u64 g_X[(PARAM_T + 1) * Param_M / 64];
    u64 S_inv[Param_M * PARAM_T * Param_M * PARAM_T / 64];
    u64 r4perm[4];
} u64_sk;

typedef struct{   
    u64 H[PUBLICKEYBYTES / 8];
} u64_pk;

void gen_key_pair(OUT u64_pk* pk, OUT u64_sk* sk, IN gf2m_tab* gf2m_tables);

void gen_rand_goppa_code(OUT gf* support_set, OUT gf* goppa_poly, OUT u64* H, IN gf2m_tab* gf2m_tables);
void get_scrambled_code(OUT u64* H_hat, OUT u64* S_inv, OUT u64* rand4P, IN u64* H);
void gen_rand_perm_mat(OUT gf* perm_mat_P, IN int row, IN u64* r);

void gen_parity_check_mat(OUT u64* H, IN gf* gf_poly, IN gf* support_set, IN gf2m_tab* gf2m_tables);

#endif