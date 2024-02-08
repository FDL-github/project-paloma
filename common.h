#ifndef COMMON_H
#define COMMON_H

#include "config.h"
#include "gf.h"

/* 쓰는 함수 */
void shuffle(OUT gf* shuffled_set, IN int set_len, IN u64* _256bits_seed_r);       // shuffle function
void gen_rand_sequence(OUT u64* rand_sequence, IN int sequence_len);               // num_of_bits 비트 랜덤 비트열을 생성하는 함수
int rand_oracle(OUT u64* seed, IN u64* msg, IN int msg_len, IN int oracle_num);    // 랜덤 오라클 역할

/* 예비 함수 및 16 비트 연산 함수*/
void shuffle_1(OUT gf* shuffled_set, IN int set_len, IN u64* _256bits_seed_r);       // 기존 shuffle function
void shuffle2(OUT u16* shuffled_set, IN int set_len, IN u16* _256bits_seed_r);     // 16비트 연산 shuffle function
void gen_rand_sequence2(OUT u16* rand_sequence, IN int sequence_len);              // num_of_bits 비트 랜덤 비트열을 생성하는 함수 _ 16비트 연산 버전

#endif