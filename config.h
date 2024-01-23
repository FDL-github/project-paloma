
#ifndef CONFIG_H
#define CONFIG_H

#include <stdint.h>
#include <stdlib.h>

#define IN
#define OUT

#define Param_M 13

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

/*
   all elements of F2m is ' z^12 + ... +a_0z^0 ' > 0 || 0 || 0 || a_12 || a_11 || .... || a_0  : 2bytes
*/
typedef uint16_t gf;

typedef uint8_t u8;
typedef uint32_t u32;
#endif