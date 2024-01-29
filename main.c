// ------------------------------------------------ 
//  gcc -O2 *.c
//  ./a.out  
// ------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "gf_tab_gen.h"
#include "rng.h"
#include "api.h"
#include "key_gen.h"
#include <time.h>

int main()
{
    /*
        유한체 연산 사전 계산 테이블 생성 및 속도 측정, 검증
    */
    gf2m_tab table;
    // 사전 계산 테이블 생성
    gen_precomputation_tab(&table);
    
    // 속도 측정
    // gf2m_performance(&table);

    // 검증
    // tab_verify_check(&table);
 
    /*
        자유롭게 쓰세요
    */
    // printf("here!");

    unsigned char       entropy_input[48];

    srand(time(NULL));

    for (int i=0; i<48; i++)
        entropy_input[i] = rand() & 0xff;
        // entropy_input[i] = i;

    randombytes_init(entropy_input, NULL, 256); 

    printf("n = %d, t = %d, k = %d\n", n, t, k);

    u64 sk64[(n*13/64) + (t* 13/64) + (13*t*13*t/64) + 4 ]={0,}; // L + g(X) + S + P 순서
    u64 pk64[13*t*(n-(13*t))/64]={0,};  

    gen_key_pair(pk64, sk64,&table);

    // gf_poly_verify()
        

    return 0;
}