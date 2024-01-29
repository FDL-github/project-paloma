#ifndef KEY_GEN_H
#define KEY_GEN_H

#include "config.h"
#include "gf.h"

void gen_key_pair(OUT u64* pk, OUT u64* sk, IN gf2m_tab* gf2m_tables);

void gen_rand_goppa_code(OUT gf* support_set, OUT gf* goppa_poly, OUT u64* H, IN gf2m_tab* gf2m_tables);
void get_scrambled_code(OUT u64* H_hat, OUT u64* S_inv, OUT u64* rand4P, IN u64* H);
void gen_rand_perm_mat(OUT gf* perm_mat_P, IN int row, IN u64* r);

void gen_parity_check_mat(OUT u64* H, IN gf* gf_poly, IN gf* support_set, IN gf2m_tab* gf2m_tables);

#endif