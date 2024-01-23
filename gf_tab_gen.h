// 연산 테이블 생성을 위한 파일

#ifndef GF_TABLE_GEN_H
#define GF_TABLE_GEN_H

#include "config.h"
#include "gf.h"

void print_all_tab(IN gf2m_tab* gf2m_tables);     // 모든 테이블 출력
void gen_precomputation_tab(OUT gf2m_tab* gf2m_tables);      // 모든 연산(mul, squ, sqrt, inv)의 사전계산 테이블 생성

void gen_mul_tab(OUT gf2m_tab* gf2m_tables);      // 곱셈용 테이블 3개 생성 
void gen_square_tab(OUT gf* square_tab);       // 제곱용 테이블 생성
void gen_sqrt_tab(OUT gf* sqrt_tab);           // 제곱근용 테이블 생성
void gen_inv_tab(OUT gf* inv_tab);             // 역원용 테이블 생성

void print_mul_tab(IN gf2m_tab* gf2m_tables);     // 곱셈용 테이블 출력
void print_square_tab(IN gf* square_tab);      // 제곱용 테이블 출력
void print_sqrt_tab(IN gf* sqrt_tab);          // 제곱근용 테이블 출력
void print_inv_tab(IN gf* inv_tab);            // 역원용 테이블 출력

void gf2m_performance(IN gf2m_tab* gf2m_tables);  // 모든 함수 시간 측정 결과 알수있도록. 
void tab_verify_check(IN gf2m_tab* gf2m_tables);  // 테이블 연산 == 일반 연산 확인

#endif