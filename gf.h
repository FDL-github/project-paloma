#ifndef GF_H
#define GF_H

#include "config.h"

#define IRR_POLY 0b10000011100001 // x^13 + x^7 + x^6 + x^5 + 1

typedef struct{   
    // Multiplication Pre-computation Table
    gf mul_11_tab[1 << GF2M_SPLIT_HIGH][1 << GF2M_SPLIT_HIGH]; // A1(z) z^7 * B1(z) z^7 mod f(z) (상위 6bit x 상위 6bit)
    gf mul_10_tab[1 << GF2M_SPLIT_HIGH][1 << GF2M_SPLIT_LOW]; // A1(z) z^7 * B0(z), A0(z) * B1(z) z^7 mod f(z) (상위 6bit x 하위 7bit)
    gf mul_00_tab[1 << GF2M_SPLIT_LOW][1 << GF2M_SPLIT_LOW]; // A0(z) * B0(z) mod f(z) (하위 7bit x 하위 7bit)
   
    gf square_tab[BITSIZE];     // Square Pre-computation Table
    gf sqrt_tab[BITSIZE];       // SquareRoot Pre-computation Table
    gf inv_tab[BITSIZE];        // Inverse Pre-computation Table

} gf2m_tab;


void gf2m_print(IN gf in);

gf gf2m_add(IN gf in1, gf in2);

gf gf2m_mul(IN gf in1, gf in2);
gf gf2m_mul_w_tab(IN gf in1, IN gf in2, IN gf2m_tab *gf2m_tables);

gf gf2m_square(IN gf in);
gf gf2m_square_w_tab(IN gf in, IN gf* square_tab);

gf gf2m_sqrt(IN gf in);
gf gf2m_sqrt_w_tab(IN gf in, IN gf* sqrt_tab);

gf gf2m_inv(IN gf in);
gf gf2m_inv_w_tab(IN gf in, IN gf* inv_tab);

#endif