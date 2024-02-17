#include "api.h"
#include "key_gen.h"
#include "encap.h"
#include "decap.h"

/**
 * @brief Generate public and secret(private) key
 *
 * @param pk pointer to output public key (a structure composed of (matrix H[n-k]))
 * @param sk pointer to output secret key
 *           (a structure composed of (support set L, goppa polynomial g(X), matrix S^{-1}, seed r for permutation matrix)
 * @param gf2m_tables precomputation table of GF(2^m) (multiplication, square, square root, inverse)
 * @return int
 */
int crypto_kem_keypair(unsigned char *pk, unsigned char *sk, const gf2m_tab *gf2m_tables)
{

#if BENCH_MODE == ON
    uint64_t cyc_s;
    uint64_t cyc_e;
    cyc_s = cycle_read();
#endif

    gen_key_pair((PublicKey *)pk, (SecretKey *)sk, gf2m_tables);

#if BENCH_MODE == ON
    cyc_e = cycle_read();
    paloma_cycle_result.gen_key = cyc_e - cyc_s;
#endif

    return 0;
}

/**
 * @brief Encapsulation (Generate ciphertext and shared key)
 *
 * @param ct pointer to output ciphertext (a structure composed of (seed r, vector s_hat))
 * @param key pointer to output shared key
 * @param pk pointer to input public key (a structure composed of (matrix H[n-k]))
 * @return int
 */
int crypto_kem_enc(unsigned char *ct, unsigned char *key, const unsigned char *pk)
{
    Word *r_hat = (Word *)ct;
    Word *s_hat = (Word *)(&ct[32]);

#if BENCH_MODE == ON
    uint64_t cyc_s;
    uint64_t cyc_e;
    cyc_s = cycle_read();
#endif

    /* Encapsulation */
    encap((Word *)key, r_hat, s_hat, (PublicKey *)pk);

#if BENCH_MODE == ON
    cyc_e = cycle_read();
    paloma_cycle_result.encap = cyc_e - cyc_s;
#endif

    return 0;
}

/**
 * @brief Decapsulation (Generate shared key using secret key and ciphertext)
 *
 * @param key pointer to output shared key
 * @param ct pointer to input ciphertext (a structure composed of (seed r, vector s_hat))
 * @param sk pointer to input secret key
 *           (a structure composed of (support set L, goppa polynomial g(X), matrix S^{-1}, seed r for permutation matrix)
 * @param gf2m_tables precomputation table of GF(2^m) (multiplication, square, square root, inverse)
 * @return int
 */
int crypto_kem_dec(unsigned char *key, const unsigned char *ct, const unsigned char *sk, const gf2m_tab *gf2m_tables)
{
    Word *r_hat = (Word *)ct;
    Word *s_hat = (Word *)(&ct[32]);

#if BENCH_MODE == ON
    uint64_t cyc_s;
    uint64_t cyc_e;
    cyc_s = cycle_read();
#endif

    /* Decapsulation */
    decap((Word *)key, (SecretKey *)sk, r_hat, s_hat, gf2m_tables);

#if BENCH_MODE == ON
    cyc_e = cycle_read();
    paloma_cycle_result.decap = cyc_e - cyc_s;
#endif

    return 0;
}
