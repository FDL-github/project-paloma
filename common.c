#include "common.h"
#include <stdio.h>
#include <time.h>
#include "include/lsh.h"
#include "lsh512.h"
#include "lsh_local.h"
#include "rng.h"
#include "include/lsh_def.h"
#include "gf.h"


/**
* @brief _256bits_seed_r 이용 하여 원소가 set_len개인 집합 셔플 
* @brief _256bits_seed_r = [64비트][64비트][64비트][64비트] .. 64비트 * 4
* @param set_len 집합 길이. paloma에서는 n으로만 들어옴. 따라서 매개변수 수정해도 됨.
* @param _256bits_seed_r 256비트 시드 r
* @param shuffled_set 셔플 결과                       
*/
void shuffle(OUT gf* shuffled_set, IN int set_len, IN u64* _256bits_seed_r){  //원소 n개 짜리 집합을 난수 r 이용 하여 셔플
    
    int i, j;
    int w = 0;
    gf seed[16];
    gf tmp;

    /* [0,1,...,n-1] 집합 생성 */
    for(i = 0; i < set_len; i++)
    {
        shuffled_set[i] = i;
    }

    /* seed 16-bit씩 분리 */    
    seed[0] = (_256bits_seed_r[0]) & 0xffff;         seed[1] = (_256bits_seed_r[0] >> 16) & 0xffff;    
    seed[2] = (_256bits_seed_r[0] >> 32) & 0xffff;   seed[3] = (_256bits_seed_r[0] >> 48) & 0xffff;
    
    seed[4] = (_256bits_seed_r[1]) & 0xffff;         seed[5] = (_256bits_seed_r[1] >> 16) & 0xffff;    
    seed[6] = (_256bits_seed_r[1] >> 32) & 0xffff;   seed[7] = (_256bits_seed_r[1] >> 48) & 0xffff;
    
    seed[8] = (_256bits_seed_r[2]) & 0xffff;         seed[9] = (_256bits_seed_r[2] >> 16) & 0xffff;    
    seed[10] = (_256bits_seed_r[2] >> 32) & 0xffff;  seed[11] = (_256bits_seed_r[2] >> 48) & 0xffff;
    
    seed[12] = (_256bits_seed_r[3]) & 0xffff;        seed[13] = (_256bits_seed_r[3] >> 16) & 0xffff;    
    seed[14] = (_256bits_seed_r[3] >> 32) & 0xffff;  seed[15] = (_256bits_seed_r[3] >> 48) & 0xffff;

    /* shuffling */
    for(i = set_len - 1; i > 0; i--)
    {
        j = ((seed[w % 16]) % (i + 1));   
        
        /* swap */          
        tmp = shuffled_set[j];
        shuffled_set[j] = shuffled_set[i];
        shuffled_set[i] = tmp;

        w = (w + 1) & 0xf;
    }
}

/**
* @brief 길이가 sequence_len 비트인 난수열 생성
* @param sequence_len 난수열 비트 길이
* @param out_vec 난수열
*/
void gen_rand_sequence(OUT u64* rand_sequence, IN int sequence_len)
{   
    
    unsigned char seed[1];
    for(int i = 0; i < (sequence_len / 64); i++)  // 64 : bit size of u64
    {
        rand_sequence[i] = 0;

        for(int j = 0; j < 8; j++) // 8 : bit size of u64 / 8
        {
            // seed[0] = (u8)rand(); // 이 부분 지우면 됨

            randombytes(seed, 1);  // 8비트 짜리 문자열 1개 나옴   // rng.h
            u64 rand_k = seed[0] & 0xff;   // 8비트 짜리 문자열 1개 나옴
            rand_sequence[i] |= (rand_k << (8 * (7 - j)));
        }
    }
}

/**
* @brief encap, decap 과정에서 사용하는 랜덤오라클
* @param msg: 오라클 입력 데이터
* @param msg_len: 오라클 입력 데이터의 비트 길이.
* @param oracle_num: 1 인경우 oracle G, 2인 경우 oracle H 사용
* @param seed: oracle 결과 (해시값)
*/
int rand_oracle(OUT u64* seed, IN u64* msg, IN int msg_len, IN int oracle_num)
{ 		  
	// LSH512을 사용하고, 출력 해시 길이를 512비트로 설정 */
	lsh_type algtype = LSH_MAKE_TYPE(1, 512);  

	// 입력 데이터앞으로 8byte 문자열이 추가되므로 8을 추가한 만큼 길이로 배열 생성
	lsh_u8 src[8 + (msg_len / 8)];

    /* oracle 입력 데이터 생성 */
    /* PALOMAGG or PALOMAHH ASCII 값*/
    src[0] = 0x50;   src[1] = 0x41;   src[2] = 0x4c;
    src[3] = 0x4f;   src[4] = 0x4d;   src[5] = 0x41;

    if(oracle_num == 1)
    {
        src[6] = 0x47;   src[7] = 0x47;
    }
    else if(oracle_num == 2)
    {
        src[6] = 0x48;   src[7] = 0x48;
    }
    else
    {
        return -1;
    }
    /* 입력받은 데이터 */
    for(int i = 0; i < (msg_len / 8); i++) 
    {
        src[8 + i] = (msg[i / 8] >> ((8 * i) % 64)) & 0xff;
    }
	
    // 출력 해시값을 512비트(512/8=64바이트) 변수 result에 저장
	lsh_u8 result[64] = {0, };

    /* 해시값 생성 */
	lsh_digest(algtype, src, 64 + msg_len, result);

    /* 해시값 출력 */
    for(int i = 0; i < 32; i++)
    {
        seed[i / 8] |= ((u64)result[i] << ((i * 8) % 64));
    }

    return 0;
}
