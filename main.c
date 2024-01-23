// ------------------------------------------------ 
//  gcc -O2 *.c
//  ./a.out  
// ------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "gf_tab_gen.h"

/* flag */
// PALOMA1 > 1
// PALOMA2 > 2
// PALOMA3 > 3
#define flag           1      

int main(){
    /*
        유한체 연산 사전 계산 테이블 생성 및 속도 측정, 검증
    */
    gf2m_tab table;
    // 사전 계산 테이블 생성
    gen_precomputation_tab(&table);
    
    // 속도 측정
    gf2m_performance(&table);

    // 검증
    // tab_verify_check(&table);
 
    /*
        자유롭게 쓰세요
    */


    return 0;
}