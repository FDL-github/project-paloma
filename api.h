#ifndef API_H
#define API_H

#include "config.h"

#define CRYPTO_PUBLICKEYBYTES PUBLICKEYBYTES
#define CRYPTO_SECRETKEYBYTES SECRETKEYBYTES
#define CRYPTO_CIPHERTEXTBYTES CIPHERTEXTBYTES
#define CRYPTO_BYTES BYTES

// Change the algorithm name
#define CRYPTO_ALGNAME "PALOMA"

#include "gf.h"

int crypto_kem_keypair(unsigned char* pk, unsigned char* sk, gf2m_tab* gf2m_tables);
int crypto_kem_enc(unsigned char* ct, unsigned char* key, const unsigned char* pk, gf2m_tab* gf2m_tables);
int crypto_kem_dec(unsigned char* key, const unsigned char* ct, const unsigned char* sk, gf2m_tab* gf2m_tables);

#endif /* api_h */