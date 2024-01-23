#include "gf.h"
#include <stdio.h>

/**
* @brief 유한체 F2m 원소 출력 함수
*/
void gf2m_print(IN gf in)
{
    int flag = 0;

    for(int i = 0; i <= 16; i++)
    {
        if(in & (1 << (Param_M - i)))
        {
            if(flag)
                printf(" + ");
            flag = 1; 

            if(Param_M-i == 0) {
                printf("1");
            } else {
                printf("Z^%d",Param_M-i);
            }
        }    
    }

    if(in == 0)
        printf("0");

    printf("\n");
}

/**
* @brief 유한체 F2m 덧셈 함수
*/
gf gf2m_add(IN gf in1, IN gf in2)
{
    return (in1 & MASKBITS) ^ (in2 & MASKBITS);
}

/**
* @brief 유한체 F2m 곱셈 함수
*/
gf gf2m_mul(IN gf in1, gf in2)
{   
    // 입력 유효성 체크
    in1 &= MASKBITS;
    in2 &= MASKBITS;

    gf result = 0;
    gf t1 = in1;
    gf t2 = in2;
    
    for(; t2; t2 >>= 1){
        result ^= (t1 * (t2 & 1));
        if (t1 & 0x1000)  // GF(2^13)에서의 최상위 비트 처리
            t1 = ((t1 << 1)) ^ IRR_POLY; 
        else
            t1 <<= 1;        
    } 

    return result;
}

/**
* @brief 유한체 F2m 테이블 이용 곱셈 함수
*/
gf gf2m_mul_w_tab(IN gf in1, IN gf in2, IN gf2m_tab* gf2m_tables)
{
    // if (IS_INVALID2(in1, in2))
    //     abort();

    // 입력 유효성 체크
    in1 &= MASKBITS;
    in2 &= MASKBITS;

    gf result = 0;

    gf int1high = (in1 >> GF2M_SPLIT_LOW) & GF2M_SPLIT_MASK_HIGH_BIT;
    gf int1low  = (in1) & GF2M_SPLIT_MASK_LOW_BIT;
    gf int2high = (in2 >> GF2M_SPLIT_LOW) & GF2M_SPLIT_MASK_HIGH_BIT;
    gf int2low  = (in2) & GF2M_SPLIT_MASK_LOW_BIT;

    result ^= gf2m_tables->mul_11_tab[int1high][int2high];
    result ^= gf2m_tables->mul_10_tab[int1high][int2low];
    result ^= gf2m_tables->mul_10_tab[int2high][int1low];
    result ^= gf2m_tables->mul_00_tab[int1low][int2low];

    return result;
}

/**
* @brief 유한체 F2m 제곱 함수
*/
gf gf2m_square(IN gf in)
{   
    // 입력 유효성 체크
    in &= MASKBITS; 

    return gf2m_mul(in, in);
}

/**
* @brief 유한체 F2m 테이블 이용 제곱 함수
*/
gf gf2m_square_w_tab(IN gf in, IN gf* square_tab)
{   
    return square_tab[in];
}

/**
* @brief 유한체 F2m 제곱근 함수
*/
gf gf2m_sqrt(IN gf in)
{   
    in &= MASKBITS;
    gf result = in;

    for(int i = 0; i < 12; i++) // a^(2^12)
		result = gf2m_square(result);

    return result;
}

/**
* @brief 유한체 F2m 테이블 이용 제곱근 함수
*/
gf gf2m_sqrt_w_tab(IN gf in, IN gf* sqrt_tab) 
{
    return sqrt_tab[in];
}

/**
* @brief 유한체 F2m 역원 계산 함수
*/
gf gf2m_inv(IN gf in)
{
    // a^(p-1) = 1 (mod p) -> a^(p-2) = a^-1 (mod p)
    gf a = in;
    gf a_2 = gf2m_square(a);              // a^2
    gf a_4 = gf2m_square(a_2);            // a^4
    gf a_6 = gf2m_mul(a_4,a_2);           // a^6
    gf a_7 = gf2m_mul(a_6,a);             // a^7
    gf a_63 = a_7;                  

    for(int i =0;i<3;i++)
        a_63 = gf2m_square(a_63);         // a^7 -> a^14-> a^28 -> a^56

    a_63 = gf2m_mul(a_63,a_7);            // a^63  = a^56 * a^7

    gf result = a_63;   

    for(int i =0;i<6;i++)
        result = gf2m_square(result);     //a^4032

    result = gf2m_mul(result,a_63);       // a^4095
    result = gf2m_square(result);         // a^8190 ... a^13 - 2 의 값을 출력함. 

    return result;
}

/**
* @brief 유한체 F2m 테이블 이용 역원 계산 함수
*/
gf gf2m_inv_w_tab(IN gf in, IN gf* inv_tab)
{
    return inv_tab[in];
}

