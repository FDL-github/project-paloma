
#ifndef CONFIG_H
#define CONFIG_H

#include <stdint.h>
#include <stdlib.h>

#define IN
#define OUT

#define Param_M 13

/**
 * @brief Set PALOMA_MODE
 * PALOMA_MODE == 0: PALOMA-128
 * PALOMA_MODE == 1: PALOMA-192
 * PALOMA_MODE == 2: PALOMA-256
 */
#define PALOMA_MODE 2

#if PALOMA_MODE == 0
    #define PARAM_N 3904
    #define PARAM_T 64
    #define PARAM_K (PARAM_N - (13 * PARAM_T))

    #define PUBLICKEYBYTES 319488
    #define SECRETKEYBYTES 93008
    #define CIPHERTEXTBYTES 136
    #define BYTES 32

#elif PALOMA_MODE == 1
    #define PARAM_N 5568
    #define PARAM_T 128
    #define PARAM_K (PARAM_N - (13 * PARAM_T))

    #define PUBLICKEYBYTES 812032
    #define SECRETKEYBYTES 355400
    #define CIPHERTEXTBYTES 240
    #define BYTES 32

#elif PALOMA_MODE == 2
    #define PARAM_N 6592
    #define PARAM_T 128
    #define PARAM_K (PARAM_N - (13 * PARAM_T))

    #define PUBLICKEYBYTES 1025024
    #define SECRETKEYBYTES 357064
    #define CIPHERTEXTBYTES 240
    #define BYTES 32

#endif

/*
   all elements of F2m is ' z^12 + ... +a_0z^0 ' > 0 || 0 || 0 || a_12 || a_11 || .... || a_0  : 2bytes
*/
typedef uint16_t gf;

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

/*
    곱셈 사전 계산 테이블을 위한 변수
*/
#define GF2M_SPLIT_LOW 7
#define GF2M_SPLIT_HIGH (Param_M - GF2M_SPLIT_LOW)              // 6
#define GF2M_SPLIT_MASK_LOW_BIT ((1 << GF2M_SPLIT_LOW) - 1)     // 127
#define GF2M_SPLIT_MASK_HIGH_BIT ((1 << GF2M_SPLIT_HIGH) - 1)   // 63

#define MAXN 6592   
#define MAXT 129    // 128차 다항식이므로 129개의 배열 필요 

/*
    입력 유효성 체크를 위한 변수
*/
#define BITSIZE (1 << Param_M) 
#define MASKBITS ((1 << Param_M) - 1) 

#endif