#ifndef MAT_MUL_H
#define MAT_MUL_H

#include "config.h"
#include "gf.h"

/* classic mc의 파라미터 형식. cm 코드의 params.h에 있음, spec.pdf에 설명 있음 */
/*
#define GFBITS 12 // Param_M 
#define SYS_N n // n 값
#define SYS_T t // t 값 
#define PK_NROWS (SYS_T*GFBITS) // mc에서 공개키 행렬의 행 크기인 mt. mc에서는 pk = T (mt x k)행렬이고. 1바이트 씩 끊어서 저장함. 따라서 행의크기는 mt
#define PK_NCOLS (SYS_N - PK_NROWS) // mc에서 공개키 행렬의 열 크기인 k(=n-mt).   
#define PK_ROW_BYTES ((PK_NCOLS + 7)/8) // pk 행렬의 각 행의 바이트 수 : Each row of T is represented as a ⌈k/8⌉-byte string
#define SYND_BYTES ((PK_NROWS + 7)/8) // 신드롬 벡터의 바이트 수 : mt(=n-k)비트 -> 바이트로 나타낸 것. 신드롬은 mt 비트임.
#define GFMASK ((1 << GFBITS) - 1) 
*/

/* 새로 정의한 파라미터 */ // 이 파라미터들은 사실 config에 있어도 될듯.
#define HP_NROWS (Param_M * PARAM_T)                // paloma에서 hp 행렬의 비트기준 행 크기인 mt(=n-k). 즉 mt비트 hp : (mt x n)행렬. 
#define HP_NCOLS PARAM_N                          // paloma에서 hp 행렬의 비트기준 열 크기인 n. 즉, nbit
#define HP_ROW_BYTES (HP_NCOLS / 8)         // hp 행렬의 각 행의 바이트 수 : [n/8⌉-byte string
#define HP_BYTES (HP_NROWS * HP_NCOLS / 8)  // hp 행렬의 총 바이트
#define PK_NROWS (Param_M * PARAM_T)                // paloma에서 공개키 pk(H_hat[n-k:n]) 행렬의 비트기준 행 크기 mt(=n-k). 
#define PK_NCOLS PARAM_K                          // paloma에서 공개키 pk(H_hat[n-k:n]) 행렬의 비트기준 열 크기 k. 즉, nbit
#define PK_BYTES ((Param_M * PARAM_T) * PARAM_K / 8)          // paloma에서 공개키 pk(H_hat[n-k:n])의 바이트 크기 : ⌈(n-k)k/8⌉-byte string
//#define SK_BYTES (2*n + 2*t + ((n-k)*(n-k))/8 + 32)    // paloma에서 개인키(L,g,s^-1,r)의 바이트 크기 : 2n+2t+⌈(n-k)^2/8⌉ +32 byte string
#define CT_BYTES (32 + (Param_M * PARAM_T)/8)
#define SYND_BYTES (HP_NROWS / 8)           // 신드롬 벡터의 바이트 수 : mt(= n-k)비트 -> 바이트로 나타낸 것. 신드롬은 mt 비트임.
#define GFMASK ((1 << Param_M) - 1)

/* 실제 사용하는 함수 */
void matXvec(OUT u64* out_vec, IN u64* in_mat, IN u64* in_vec, IN int rownum, IN int colnum); // 행렬 x 벡터 
void matXmat(OUT u64* out_mat, IN u64* in_mat1, IN u64* in_mat2, IN int mat1_rownum, IN int mat1_colnum, IN int mat2_colnum); // 행렬 X 행렬 기존 코드
void gen_identity_mat(OUT u64* I_Mat, IN int row); // 항등행렬 생성
int gaussian_row(OUT u64* systematic_mat, IN u64* in_mat); // 가우시안 소거법. 입력 행렬을 행 벡터 행렬로 바꿔서 연산. 기존 파라미터와 다름.


#endif