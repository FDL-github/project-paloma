#include "mat_mul.h"
#include <stdio.h>
#include <time.h>
#include <string.h>
/* 고민했던 내용 */
// 사실 0 || 0 || 0 || a_12 || a_11 || .... || a_0 형태이니까 앞에 3개는 연산 필요없다고 생각했음
// 0 || 0 || 0 || a_12 || a_11 || .... || a_0라 생각하고 코드 짬.
// 이건 공개된 사실이니 적용시키면 시간 줄일 수 있을 것 같은데 어케해야할지..
// 아니다.. 행의 수는 t개, 열의 수 n개인가... 
// 근데 이거 다 잘못된 생각인듯.. 배열에 0 || 0 || 0 || a_12 || a_11 || .... || a_0이 모두 드가는게 아니라 a_12~a_0만 들어간다..?

// 결론 : in_vec의 비트가 1인 애들이랑 연산하는 in_mat의 열 벡터를 out_vec에 저장하고 xor하는거인듯..

/**
* @brief 행렬 x 벡터 
* @param in_mat  입력 행렬
* @param in_vec  입력 벡터
* @param rownum  입력 행렬의 행 개수
* @param colnum  입력 행렬의 열 개수 (입력 벡터의 길이)
* @param out_vec 출력 벡터
*/
void matXvec(OUT u64* out_vec, IN u64* in_mat, IN u64* in_vec, IN int rownum, IN int colnum){ // 모든 64 비트는 열단위.
    // out_vec의 모든 값은 0으로 채워져서 들어옴.
    int num_of_64bits_in_row = rownum >> 6; // 행에 몇개의 u64가 있는지 계산..
    int col_index;

    for(int i = 0; i < colnum; i++)   
    {       
        col_index = (i * num_of_64bits_in_row);

        if((in_vec[i >> 6] >> (i % 64)) & 1) 
        {   
            // r_0이 1이면 in_mat에서 각 u64 행의 r_0이랑 연신헤야 하는 값이랑 연산 돌림.
            // 즉 num_of_64bits_in_row 만큼 연산 돌려야 함. 
            for(int j = 0; j < num_of_64bits_in_row; j++) 
                out_vec[j] ^= in_mat[col_index + j]; // in_mat에서각 u64 행의 r_0랑 연신헤야 하는 값 모두랑 연산 돌림.
           
        }
    }
}

/**
* @brief 행렬 x 행렬
* @brief 기존 코드에서 변수만 및 양식 변경
* @param in_mat1  입력 행렬1
* @param in_mat2  입력 행렬2
* @param mat1_rownum 입력 행렬1의 행 개수
* @param mat1_colnum 입력 행렬1의 열 개수 (입력 행렬2의 행개수)
* @param mat2_colnum 입력 행렬2의 열 개수
* @param out_mat 출력 행렬
*/
void matXmat(OUT u64* out_mat, IN u64* in_mat1, IN u64* in_mat2, IN int mat1_rownum, IN int mat1_colnum, IN int mat2_colnum){ // 모든 64비트는 열단위.
    int mat1_row64num = mat1_rownum >> 6;
    int mat2_row64num = mat1_colnum >> 6;

    u64 tmp_mat[MAXN >> 6] = {0,};

    for(int i = 0; i < mat2_colnum; i++) // 두번째 행렬 열개수만큼 반복
    {
        u64 tmp2_mat[MAXN >> 6] = {0,};

        for(int j = 0; j < mat2_row64num; j++)
        {
            tmp_mat[j] = in_mat2[(i * mat2_row64num) + j];            
        }

        matXvec(tmp2_mat, in_mat1, tmp_mat, mat1_rownum, mat1_colnum);

        for(int j = 0; j < (mat1_row64num); j++)
        {
            out_mat[(i * mat1_row64num) + j] = tmp2_mat[j];
        }
    }
}


/**
* @brief 항등행렬 생성
* @param row  행렬의 행 개수
*/
void gen_identity_mat(OUT u64* I_Mat, IN int row){
    int rowgf = row << 6;
    u64 a = 1;

    for(int i = 0; i < row; i++)
    {
        I_Mat[(i * rowgf) + (row << 6)] = a << (i % 64);
    }
}

/**
* @brief gaussian elimination
* @brief 정적할당. 열 벡터 행렬(in_mat) -> 행 벡터 행렬(HPmat_row) -> 2차원 행렬(HP_hat) -> 열 벡터 행렬(out) 총 4번 변환
* @param in_mat  입력 행렬. 열 벡터 형태로 들어옴
* @param param_n 팔로마 파라미터 n 
* @param param_t 팔로마 파라미터 t
* @param systematic_mat systematic 행렬 (열 벡터 행렬)
*/
// 만약 gf(=2바이트)에 저장하면 h_hat은 (mt/16 행 x n/16열)이 된다.
// hp(h_hat)는 비트기준 mt * n 행렬임.. mt*n (= HP_NROWS * HP_NCOLS) 비트 이걸 u64(= 8바이트)에 저장하면 h_hat은 (mt/64 행 x n/64열)이 되는 것
// HP_NROWS * HP_NCOLS 비트 = HP_BYTES 바이트. 이걸 u64(= 8바이트)로 저장하면 아래와 같음. 
// 만약 gf(2바이트에 저장하면) gf HPmat_row[HP_BYTES / 2] = {0,};  unsigned char(1바이트에 저장하면) gf HPmat_row[HP_BYTES] = {0,};  
int gaussian_row(OUT u64* systematic_mat, IN u64* in_mat)
{

    u64 HP_hat[HP_NROWS][HP_NCOLS / 64]={{0,},};
    u64 tmp2 = 0;

    int cnt = 0;
    for(int j = 0; j < PARAM_N; j++)
    {   
        for(int i = 0; i < (Param_M * PARAM_T); i++)
        {
            HP_hat[i][cnt] |= (((in_mat[(i + (j * 13 * PARAM_T)) / 64] >> (i % 64)) & 1) << (j % 64));
        }
        if(j % 64 == 63)
        {   
            cnt ++;
        }
    }  

    /* Gaussian elimination */
    int r = 0;
    int flag = 0;
    while(r < HP_NROWS)
    {
        flag = 0;

        for(int i = r; i < HP_NROWS; i++){
            if(((HP_hat[i][r / 64] >> (r % 64)) & 1) == 1)
            {  
                if(i != r)
                {
                    for(int j = 0; j < (HP_NCOLS / 64); j++)
                    {  // HP_hat행렬 i,r 번째 행 스왑. 
                        tmp2 = HP_hat[i][j];
                        HP_hat[i][j] = HP_hat[r][j];
                        HP_hat[r][j] = tmp2;
                    }
                }
                flag = 1;
            }
            if(flag == 1)
            {        
                for(int j = 0; j < HP_NROWS; j++)
                {
                    if((j != r) && ((HP_hat[j][r / 64] >> (r % 64)) & 1))
                    {
                        for(int x = 0; x < (HP_NCOLS / 64); x++)
                        {
                            HP_hat[j][x] ^=  HP_hat[r][x];
                        }
                    }
                }
                break;
            }
        }
        if(flag == 0)
        {
            return -1;
        }
            
        r++;
    }

    memset(systematic_mat, 0, (sizeof(u64)) * (PARAM_N * Param_M * (PARAM_T / 64)));

    for(int j = 0; j < HP_NCOLS; j++)
    {
        for(int i = 0; i < HP_NROWS; i++)
        {
            systematic_mat[(i / 64) + (j * (HP_NROWS / 64))] |= ((u64)((HP_hat[i][j / 64] >> (j % 64)) & 1) << (i % 64));
        }
    }

    return 0;
}

