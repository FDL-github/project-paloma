#ifndef ENCAP_H
#define ENCAP_H

#include "config.h"
#include "gf.h"

void encrypt(OUT u64* syndrome, IN u64* pk, IN u64* e);
void gen_rand_err_vec(OUT u64* error_vector, IN u64* seed);
// void GenRandErrVec(OUT u64* errorvec, IN u64* seed, IN int n, IN int t);
// void Encrypt(OUT u64* synd, IN u64* pk, IN u64* e, IN int n, IN int k);


#endif
