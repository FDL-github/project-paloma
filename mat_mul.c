#include "mat_mul.h"
#include <stdio.h>
#include <time.h>
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
int Gaussian_row(OUT u64* systematic_mat, IN u64* in_mat, IN int param_n, IN int param_t){

    u64 HPmat_row[HP_BYTES / 8] = {0,};  
    u64 HP_hat[HP_NROWS][HP_NCOLS / 64]={{0,},};
    //u64 HP_hat[13*MAXT][MAXN/64]={{0,},};
    u64 tmp2;
    // printf("hi1");

    /* 원활한 Gaussian을 위해 열단위로 표현된 행렬을 행단위로 변경 */
    /* HPmat_row <- in_mat(= input HP) 복사 */
    // 사실 HPmat_row 랑 HP를 동시에 선언할 필요 없이, 한번에 in+mat -> HPmat_row[HP_NROWS/64][HP_NCOLS/64] 형식으로 바꿔야 함.
    for(int j = 0; j < param_n; j++)
    {
        for(int i = 0; i < (Param_M * param_t); i++)
        {
            HPmat_row[((param_n * i) + j) / 64] |= (((in_mat[(i + (j * 13 * param_t))/64] >> (i % 64)) & 1)<<(j % 64));
        }
    }    

    // printf("hi2");

    
    /* Hp_hat <- HPmat_row 복사 */ 
    for(int i = 0; i < (HP_NROWS * (HP_NCOLS / 64)); i++)
    {
        HP_hat[i / (HP_NCOLS / 64)][i % (HP_NCOLS / 64)] = HPmat_row[i];
    }
    // printf("hi3");


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
            printf("here -1");
            return -1;
        }
            
        r++;
    }

    for(int j = 0; j < HP_NCOLS; j++)
    {
        for(int i = 0; i < HP_NROWS; i++)
        {
            systematic_mat[(i / 64) + (j * (HP_NROWS / 64))] |= ((u64)((HP_hat[i][j / 64] >> (j % 64)) & 1) << (i % 64));
        }
    }
    return 0;
}


/**
* @brief gaussian elimination
* @brief 기존 함수에서 동적할당으로만 바꾼 것. 입력행렬이 행 벡터 형태로 들어옴. 만약 Gaussian_row1 안 되면 이거 사용.
* @param in_mat  입력 행렬 .. 행 벡터 형식
* @param rownum  입력 행렬 행개수
* @param colnum  입력 행렬 열개수
* @param out     출력
*/
int Gaussian_row2(OUT u64* systematic_mat, IN u64* in_mat, IN int rownum, IN int colnum){
    int row64num = rownum >> 6;
    int col64num = colnum >> 6; //int row64num=rownum/64;
    int r = 0; // 가우스 소거법의 현재 단계
    int flag; // 현재 단계에서 행 교환을 수행했는지 여부

    u64 tmp2;

    u64 **M = (u64 **)calloc(rownum, sizeof(u64 *));
    for (int i = 0; i < rownum; i++) 
    {
        M[i] = (u64 *)calloc(col64num, 8); // 8 = sizeof(u64)
    }
    
    for(int i = 0; i < (rownum * col64num); i++) // 입력행렬 M에 복사
    {
        M[i / col64num][i % col64num] = in_mat[i];
    }

    while(r < rownum) // 각 행에 대한 가우스 소거법 수행
    {
        flag = 0; 

        for(int i = r; i < rownum; i++) // 현재 행(r) 이후의 모든 행에 대해 반복. 현재 행 이후의 각 행에 대해 'r'열에 있는 값이 0이 아닌 행 찾기
        {   
            if(((M[i][r / 64] >> (r % 64)) & 1) == 1) 
            // 현재 행(i)에서 현재 열(r)에 있는 비트가 1인지 확인. 행렬의 각 열에서 현재 행 이후의 행을 검사하여 0이 아닌 값을 찾는 과정
            {  
                if(i != r)
                {
                    for(int j = 0; j < col64num; j++)
                    {  // M행렬 i,r 번째 행 스왑. 
                        tmp2 = M[i][j];
                        M[i][j] = M[r][j];
                        M[r][j] = tmp2;
                    }
                }

                flag = 1; 
            }

            if(flag == 1) // 현재 열(r)에서 0이 아닌 값을 찾았으므로 해당 열의 다른 행들을 0으로 만들기
            {        
                for(int j = 0; j < rownum; j++) // 현재 열(r)의 모든 행에 대해 반복
                {
                    if((j != r) && ((M[j][r / 64] >> (r % 64)) & 1)) // (현재 행 != 현재 열)이고 1인 값을 가지고 있는 행.
                    {
                        for(int x = 0; x < col64num; x++) // 현재 행의 각 비트에 대해 열을 건너 뛰며 반복
                        {
                            M[j][x] ^=  M[r][x]; // 현재 행과 r행의 모든 열 값 xor?
                        }
                    }
                }
                break; // 현재 열에 대한 소거 작업 완료?
            }
        }

        if(flag == 0) // 소거법 진행 불가
        {   
            for (int i = 0; i < rownum; i++) 
            {
                free(M[i]);
            }
            free(M);
            return -1;
        }

        r++; // 다음 행으로 이동?
    }

    for(int j = 0; j < colnum; j++)
    {
        for(int i = 0; i < rownum; i++)
        {
            systematic_mat[(i / 64) + (j * row64num)] |= ((u64)((M[i][j / 64] >> (j % 64)) & 1) << (i % 64));
        }
    }

    return 0;

}



int Gaussian_row3(OUT u64* out, IN u64* in_mat, IN int rownum, IN int colnum){
    int row64num = rownum >> 6;   // rownum 13t mt
    int col64num = colnum >> 6; // colnum  (n/64)
    
    u64 M[13*MAXT][MAXN/64]={{0,},};  //H  행렬로 하여 역행렬 계산.
    u64 tmp2;
    
    for(int i=0;i<(rownum*col64num);i++){
        M[i/col64num][i%col64num] = in_mat[i];
    }

    int r=0;
    int flag=0;
    while(r<rownum){     
        flag=0;
        for(int i=r;i<rownum;i++){
            if(((M[i][r/64]>>(r%64))&1)==1)
            {  
                if(i!=r) {
                    for(int j=0;j<col64num;j++) {  // M행렬 i,r 번째 행 스왑. 
                        tmp2 = M[i][j];
                        M[i][j] = M[r][j];
                        M[r][j] = tmp2;
                    }
                }
                flag=1;
            }
            if(flag==1){        
                for(int j=0;j<rownum;j++){
                    if((j!=r)&&((M[j][r/64]>>(r%64))&1)){
                        for(int x=0;x<col64num;x++){
                            M[j][x] ^=  M[r][x];
                        }
                    }
                }
                break;
            }
        }
        if(flag==0)
            return -1;
        r++;
    }

    int count =0;
    for(int j=0;j<colnum;j++){
        for(int i=0;i<rownum;i++){
            out[(i/64)+(j*row64num)] |= ((u64)((M[i][j/64]>>(j%64))&1)<<(i%64));
        }
    }

    return 0;

}
////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////                                  여러 케이스로 나눠서 구현                                             /////
////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
* @brief 행렬 x 벡터 16비트 연산
* @param in_mat  입력 행렬
* @param in_vec  입력 벡터
* @param rownum  입력 행렬의 행 개수
* @param colnum  입력 행렬의 열 개수 (입력 벡터의 길이)
* @param out_vec 출력 벡터
*/
/*
void matXvec_16(OUT gf* out_vec, IN gf* in_mat, IN gf* in_vec, IN int rownum, IN int colnum){ // 모든 16 비트는 열단위.
    // out_vec의 모든 값은 0으로 채워져서 들어옴.
    int num_of_16bits_in_row = rownum >> 4; // 행에 몇개의 gf가 있는지 계산

    for(int i = 0; i < colnum; i++) 
    {   
        int col_index = (i * num_of_16bits_in_row); // 사실 row_index로 표현해야할지, col_index라 해야할지... 행렬 관점에선 col이 맞긴한데,,

        if((in_vec[i >> 4] >> (i % 16)) & 1) // vec 행 중에 비트값이 1인 값이랑만 연산 
        {
            for(int j = 0; j < num_of_16bits_in_row; j++) // for(int j = 0; j < num_of_16bits_in_row; j++) 대신 3부터 해도 됨.
                out_vec[j] ^= in_mat[col_index + j];
        }
    }
}
*/

/**
* @brief 행렬 x 행렬
* @brief 행렬의 크기 동적으로 수정 및 16비트 연산
* @param in_mat1  입력 행렬1
* @param in_mat2  입력 행렬2
* @param mat1_rownum 입력 행렬1의 행 개수
* @param mat1_colnum 입력 행렬1의 열 개수 (입력 행렬2의 행개수)
* @param mat2_colnum 입력 행렬2의 열 개수
* @param out_mat 출력 행렬
*/
/*
void matXmat_16(OUT gf* out_mat, IN gf* in_mat1, IN gf* in_mat2, IN int mat1_rownum, IN int mat1_colnum, IN int mat2_colnum){ // 모든 64비트는 열단위.
    int mat1_row16num = mat1_rownum >> 4;
    int mat2_row16num = mat1_colnum >> 4;

    //gf tmp[MAXN >> 4]={0,};
    gf *tmp_mat = (gf*)calloc(mat2_row16num, 2); // u64 *tmp = (u64*)calloc(mat2_row64num, sizeof(u64));

    for(int i = 0; i < mat2_colnum; i++){// 두번째 행렬 열개수만큼 반복
        
        //gf tmp2[MAXN >> 4] = {0,};
        gf *tmp2_mat = (gf*)calloc(mat1_row16num, 2); // u64 *tmp = (u64*)calloc(mat2_row64num, sizeof(u64));

        for(int j = 0; j < mat2_row16num; j++){
            tmp_mat[j] = in_mat2[(i * mat2_row16num) + j];            
        }

        matXvec(tmp2_mat, in_mat1, tmp_mat, mat1_rownum, mat1_colnum);

        for(int j = 0; j < (mat1_row16num); j++){
            out_mat[(i * mat1_row16num) + j] = tmp2_mat[j];
        }
        free(tmp2_mat);
    }
    free(tmp_mat);
}
*/

/**
* @brief 행렬 x 행렬
* @brief 행렬의 크기 동적으로 수정한 코드
* @param in_mat1  입력 행렬1
* @param in_mat2  입력 행렬2
* @param mat1_rownum 입력 행렬1의 행 개수
* @param mat1_colnum 입력 행렬1의 열 개수 (입력 행렬2의 행개수)
* @param mat2_colnum 입력 행렬2의 열 개수
* @param out_mat 출력 행렬
*/
/*
void matXmat2(OUT u64* out_mat, IN u64* in_mat1, IN u64* in_mat2, IN int mat1_rownum, IN int mat1_colnum, IN int mat2_colnum){ // 모든 64비트는 열단위.
    int mat1_row64num = mat1_rownum >> 6;
    int mat2_row64num = mat1_colnum >> 6;

    u64 *tmp_mat = (u64*)calloc(mat2_row64num, 8); // u64 *tmp = (u64*)calloc(mat2_row64num, sizeof(u64));

    for(int i = 0; i < mat2_colnum; i++) // 두번째 행렬 열개수만큼 반복
    {
        u64 *tmp2_mat = (u64*)calloc(mat1_row64num, 8);

        for(int j = 0; j < mat2_row64num; j++)
        {
            tmp_mat[j] = in_mat2[(i * mat2_row64num) + j];            
        }

        matXvec(tmp2_mat, in_mat1, tmp_mat, mat1_rownum, mat1_colnum);

        for(int j = 0; j < (mat1_row64num); j++)
        {
            out_mat[(i * mat1_row64num) + j] = tmp2_mat[j];
        }
        free(tmp2_mat);
    }

    free(tmp_mat);
}
*/

// 16비트 연산
/*
void gen_identity_mat_16(OUT gf* I_Mat, IN int row){
    int rowgf = row << 4;
    gf a = 1;

    for(int i = 0; i < row; i++)
    {
        I_Mat[(i * rowgf) + (row << 4)] = a << (i % 16);
    }
}
*/

////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////                           기타 사용하지 않는 코드 고민 내용                                              /////
////////////////////////////////////////////////////////////////////////////////////////////////////////////


/*  // matXvec1 이랑 동일, 고민한 내용 써놓은 것, 
void matXvec2(OUT u64* out_vec, IN u64* in_mat, IN u64* in_vec, IN int rownum, IN int colnum){ // 모든 64 비트는 열단위.
    int num_of_64bits_in_row = rownum >> 6; // 행에 몇개의 u64가 있는지 계산..

    // mat의 colnum이 in_vec의 행의수와 같음. in_mat X in_vec -> in_mat의 col과 in_vec의 row 같아야 함.
    // 이건 사실, vec의 행 수만큼 곱셈을 하는 것임.
    // 총 행의 수를 64로 나눠서. 만약 총 행의 수가 3904면 3904/64 = 61개 연산
    for(int i = 0; i < colnum; i++)   
    {       
        int col_index = (i * num_of_64bits_in_row);
        // vec의 행 61개 중 in_vec[0] 부터 in_vec[60]까지 연산.
        // in_vec[x]에 64비트(r_63 || r_62 || ... || r_0) 있는데 이중 비트값이 1인 값 이랑만 연산
        // r_0 부터 연산
        if((in_vec[i >> 6] >> (i % 64)) & 1) 
        {   
            // r_0이 1이면 in_mat에서 각 u64 행의 r_0이랑 연신헤야 하는 값이랑 연산 돌림.
            // 즉 num_of_64bits_in_row 만큼 연산 돌려야 함.        
            for(int j = 0; j < num_of_64bits_in_row; j++) 
                out_vec[j] ^= in_mat[col_index + j]; // in_mat에서각 u64 행의 r_0이랑 연신헤야 하는 값 모두랑 연산 돌림.
                //사실 여기서 000이 16 주기로 계속 나옴. 이거 연산 안 하게하면 좋을듯한데.. 
                //라고 생각했지만. 이건 f_2^13 원소들로 이루어져 있을때!!만 해당함. 근데 실제로 그렇지 않나?
                
        }
    }
}*/


/* 완전히 잘못 짠 코드*/
/*
void matXvec3(OUT gf* out_vec, IN gf* in_mat, IN gf* in_vec, IN int rownum, IN int colnum){ // 모든 16 비트는 열단위.
    int num_of_16bits_in_row = rownum >> 4; 

    for(int i = 0; i < colnum; i++) 
    {   
        if((in_vec[i >> 4] >> (i % 16)) & 1) // vec 행 중에 비트값이 1인 값이랑만 연산 
        {   
            for(int j = 0; j < num_of_16bits_in_row; j++)
            {
                int row_index_of_out_vec = j << 4; // j * 16
                
                //out_vec[row_index_of_out_vec] ^= (in_mat[j] >> 15) & 1;
                //out_vec[row_index_of_out_vec+1] ^= (in_mat[j] >> 14) & 1;
                //out_vec[row_index_of_out_vec+2] ^= (in_mat[j] >> 13) & 1;
                out_vec[row_index_of_out_vec+3] ^= (in_mat[j] >> 12) & 1;
                out_vec[row_index_of_out_vec+4] ^= (in_mat[j] >> 11) & 1;
                out_vec[row_index_of_out_vec+5] ^= (in_mat[j] >> 10) & 1;
                out_vec[row_index_of_out_vec+6] ^= (in_mat[j] >> 9) & 1;
                out_vec[row_index_of_out_vec+7] ^= (in_mat[j] >> 8) & 1;
                out_vec[row_index_of_out_vec+8] ^= (in_mat[j] >> 7) & 1;
                out_vec[row_index_of_out_vec+9] ^= (in_mat[j] >> 6) & 1;
                out_vec[row_index_of_out_vec+10] ^= (in_mat[j] >> 5) & 1;
                out_vec[row_index_of_out_vec+11] ^= (in_mat[j] >> 4) & 1;
                out_vec[row_index_of_out_vec+12] ^= (in_mat[j] >> 3) & 1;
                out_vec[row_index_of_out_vec+13] ^= (in_mat[j] >> 2) & 1;
                out_vec[row_index_of_out_vec+14] ^= (in_mat[j] >> 1) & 1;
                out_vec[row_index_of_out_vec+15] ^= in_mat[j] & 1;
            }      
        }
    }
}
*/

