#include "gf_tab_gen.h"
#include <stdio.h>
#include <time.h>
#include <stdlib.h>

/**
* @brief 생성한 테이블 출력함수 (확인용)
*/
void print_all_tab(IN gf2m_tab* gf2m_tables)
{
    print_mul_tab(gf2m_tables); 
    print_square_tab(gf2m_tables->square_tab);
    print_sqrt_tab(gf2m_tables->sqrt_tab);
    print_inv_tab(gf2m_tables->inv_tab);
}

/**
* @brief 사전계산 테이블 생성함수
*/
void gen_precomputation_tab(OUT gf2m_tab* gf2m_tables)
{
    gen_mul_tab(gf2m_tables); 
    gen_square_tab(gf2m_tables->square_tab);
    gen_sqrt_tab(gf2m_tables->sqrt_tab);
    gen_inv_tab(gf2m_tables->inv_tab);
}

/**
* @brief 유한체 곱셈 사전계산 테이블 생성함수
*/

void gen_mul_tab(OUT gf2m_tab* gf2m_tables)
{
    gf A1_z7, B1_z7;
    
    for(gf i = 0; i < (1 << GF2M_SPLIT_LOW); i++)
    {
        for(gf j = 0; j < (1 << GF2M_SPLIT_LOW); j++)
        {
            gf2m_tables->mul_00_tab[i][j] = gf2m_mul(i, j);                      // 하위 7bit간 연산
            
            if((i < (1 << GF2M_SPLIT_HIGH)) && (j < (1 << GF2M_SPLIT_HIGH)))
            {
                A1_z7 = (i << GF2M_SPLIT_LOW);           // A1_z7 = i << 7 : 곱하기 z^7
                B1_z7 = (j << GF2M_SPLIT_LOW);           // B1_z7 = j << 7 : 곱하기 z^7

                gf2m_tables->mul_11_tab[i][j] = gf2m_mul(A1_z7, B1_z7);          //상위 6bit간 연산
            }

            if(i < (1 << GF2M_SPLIT_HIGH))
            {
                A1_z7 = (i << GF2M_SPLIT_LOW);           // A1_z7 = i << 7 : 곱하기 z^7
                
                gf2m_tables->mul_10_tab[i][j] = gf2m_mul(A1_z7, j);              //상위 6bit x 하위 7bit
            }
        }
    }    
}

/**
* @brief 유한체 제곱 사전계산 테이블 생성함수
*/
void gen_square_tab(OUT gf* square_tab)
{      
    for(gf i = 0; i < BITSIZE; i++)
    {
        square_tab[i] = gf2m_square(i);           //모든 원소 제곱 테이블
    }
}

/**
* @brief 유한체 제곱근 사전계산 테이블 생성함수
*/
void gen_sqrt_tab(OUT gf* sqrt_tab)
{
    for(gf i = 0; i < BITSIZE; i++)
    {
        sqrt_tab[i] = gf2m_sqrt(i);           //모든 원소 제곱근 테이블
    }
}

/**
* @brief 유한체 역원 사전계산 테이블 생성함수
*/
void gen_inv_tab(OUT gf* inv_tab)
{
    for(gf i = 0; i < BITSIZE; i++)
    {
        inv_tab[i] = gf2m_inv(i);              //모든 원소 역원 테이블
    }
}


/**
* @brief 유한체 곱셈 사전계산 테이블 출력함수
*/
void print_mul_tab(IN gf2m_tab* gf2m_tables)
{   
    printf("GF2M MUL TABLE ={ {");
    
    int count = 0;
    
    for(gf i = 0; i < (1 << GF2M_SPLIT_HIGH); i++)
    {
        printf("{");
        for(gf j = 0; j < (1 << GF2M_SPLIT_HIGH); j++)
        {
            printf("0x%04x", gf2m_tables->mul_11_tab[i][j]);
            if(j != GF2M_SPLIT_MASK_HIGH_BIT)
                printf(", ");
            count++;
            if((count % 20) == 0)    
                printf("\n      ");
        }
        printf("}");
        if(i != GF2M_SPLIT_MASK_HIGH_BIT) 
            printf(",");
    }
    printf("},\n");

    printf("   {");
    count = 0;
    for(gf i = 0; i < (1 << GF2M_SPLIT_HIGH); i++)
    {
        printf("{");
        for(gf j = 0; j < (1 << GF2M_SPLIT_LOW); j++)
        {
            printf("0x%04x", gf2m_tables->mul_10_tab[i][j]);
            if(j != GF2M_SPLIT_MASK_LOW_BIT)
                printf(", ");
            count++;
            if((count % 20) == 0)    
                printf("\n      ");
        }
        printf("}");
        if(i != GF2M_SPLIT_MASK_HIGH_BIT) printf(",");
    }
    printf("},\n");

    printf("   {");

    count = 0;
    for(gf i = 0; i < (1 << GF2M_SPLIT_LOW); i++)
    {
        printf("{");
        for(gf j = 0; j < (1 << GF2M_SPLIT_LOW); j++)
        {
            printf("0x%04x", gf2m_tables->mul_00_tab[i][j]);
            if(j != GF2M_SPLIT_MASK_HIGH_BIT)
                printf(", ");
            count++;
            if((count % 20) == 0)    
                printf("\n      ");
        }
        printf("}");
        if(i != GF2M_SPLIT_MASK_HIGH_BIT) printf(",");
    }
    printf("}};\n");
}

/**
* @brief 유한체 제곱 사전계산 테이블 출력함수
*/
void print_square_tab(IN gf* square_tab)
{
    printf("GF2M MUL TABLE = {");
    
    for(gf j = 0; j < BITSIZE; j++)
    {
        printf("0x%04x", square_tab[j]);
        if(j != 0x1FFF)
            printf(", ");
        if((j + 1) % 20 ==0)    
            printf("\n      ");
    }
    printf("};\n");
}

/**
* @brief 유한체 제곱근 사전계산 테이블 출력함수
*/
void print_sqrt_tab(IN gf* sqrt_tab)
{      
    printf("gf2m sqrt_tab = {");
    
    for(gf j = 0; j < BITSIZE; j++)
    {
        printf("0x%04x", sqrt_tab[j]);
        if(j != 0x1FFF)
            printf(", ");
        if((j + 1) % 20 == 0)    
            printf("\n      ");
    }
    printf("};\n");
}

/**
* @brief 유한체 역원 사전계산 테이블 출력함수
*/
void print_inv_tab(IN gf* inv_tab)
{
    printf("gf2m inv_tab [(1<<GF2M_DEG)] = {");
    
    for(gf j = 0; j < BITSIZE; j++)
    {
        printf("0x%04x", inv_tab[j]);
        if(j != 0x1FFF)
            printf(", ");
        if((j + 1) % 20 == 0)    
            printf("\n      ");
    }       
    printf("};\n");
}

/**
* @brief 유한체 연산 테이블 성능 측정 함수
*/
void gf2m_performance(IN gf2m_tab* gf2m_tables)
{
	clock_t start, end;
	double res;
    int count = 10000000;
    gf finite_a, finite_b, finite_c, finite_d;
    srand(time(NULL));

    /**************  덧셈 속도  **************/
    finite_a = rand()%0x1FFF;
    finite_b = rand()%0x1FFF;

    // printf("a = ");    gf2m_print(finite_a);
    // printf("b = ");    gf2m_print(finite_b);
    // printf("\n");

    start = clock();
	for(int i=0; i<count; i++ )
	{
        finite_a = gf2m_add(finite_a, finite_b);
        //finite_c = gf2m_Add(finite_a,finite_b);
	}
	end = clock();
	res = (double)(end - start)/CLOCKS_PER_SEC;

    printf("%lf sec. ", res);
    printf("%d-ADD over GF(2^m)\n",count);
    
    /**************  곱셈 속도 비교 **************/
    finite_a = rand()%0x1FFF;
    finite_b = rand()%0x1FFF;

    start = clock();
	for(int i=0; i<count; i++ )
	{   
        finite_a = gf2m_mul(finite_a, finite_b);
	}
	end = clock();
	res = (double)(end - start)/CLOCKS_PER_SEC;

    printf("%lf sec. ", res);
    printf("%d-MUL over GF(2^m)\n",count);

    start = clock();
	for(int i=0; i<count; i++ )
	{    
        finite_a = gf2m_mul_w_tab(finite_a, finite_b, gf2m_tables);           
	}
	end = clock();
	res = (double)(end - start)/CLOCKS_PER_SEC;

    printf("%lf sec. ", res);
    printf("%d-MUL over GF(2^m) with TABLE\n",count);

    /**************  제곱 속도 비교 **************/
    finite_a = rand()%0x1FFF;

	start = clock();
	for(int i=0; i<count; i++ )
	{
        finite_a = gf2m_square(finite_a);
	}
	end = clock();
	res = (double)(end - start)/CLOCKS_PER_SEC;

    printf("%lf sec. ", res);
    printf("%d-SQU over GF(2^m)\n",count);

	start = clock();
	for(int i=0; i<count; i++ )
	{    
        finite_a = gf2m_square_w_tab(finite_a, gf2m_tables->square_tab);
	}
	end = clock();
	res = (double)(end - start)/CLOCKS_PER_SEC;

    printf("%lf sec. ", res);
    printf("%d-SQU over GF(2^m) with TABLE\n",count);

    /**************  제곱근 속도 비교  **************/
    finite_a = rand()%0x1FFF;

	start = clock();
	for(int i=0; i<count; i++ )
	{
        finite_a = gf2m_sqrt(finite_a);
	}
	end = clock();
	res = (double)(end - start)/CLOCKS_PER_SEC;

    printf("%lf sec. ", res);
    printf("%d-SQRT over GF(2^m)\n",count);

	start = clock();
	for(int i=0; i<count; i++ )
	{    
        finite_a = gf2m_sqrt_w_tab(finite_a, gf2m_tables->sqrt_tab);
	}
	end = clock();
	res = (double)(end - start)/CLOCKS_PER_SEC;

    printf("%lf sec. ", res);
    printf("%d-SQRT over GF(2^m) with TABLE\n",count);

    /**************  역원 속도 비교 **************/
    finite_a = rand()%0x1FFF;

	start = clock();
	for(int i=0; i<count; i++ )
	{
        finite_a = gf2m_inv(finite_a);
	}
	end = clock();
	res = (double)(end - start)/CLOCKS_PER_SEC;

    printf("%lf sec. ", res);
    printf("%d-INV over GF(2^m)\n",count);

	start = clock();
	for(int i=0; i<count; i++ )
	{    
        finite_a = gf2m_inv_w_tab(finite_a, gf2m_tables->inv_tab);
	}
	end = clock();
	res = (double)(end - start)/CLOCKS_PER_SEC;

    printf("%lf sec. ", res);
    printf("%d-INV over GF(2^m) with TABLE\n",count);
}


void tab_verify_check(IN gf2m_tab* gf2m_tables)
{
    int count = 1000000;
    srand(time(NULL));

    /* 곱셈 검증 */
    gf finite_a, finite_b;
    finite_a = rand()%0x1FFF;
    finite_b = rand()%0x1FFF;
    gf finite_c = finite_a;
    
    // printf("a = ");    gf2m_print(finite_a);
    // printf("b = ");    gf2m_print(finite_b);
    // printf("\n");

	for(int i=0; i<=count; i++ )
	{
        finite_a = gf2m_mul(finite_a,finite_b);
        finite_c = gf2m_mul_w_tab(finite_c, finite_b, gf2m_tables);
        if(finite_a!=finite_c)
            printf("error!!\n");
	}
    printf("곱셈 체크 완료\n");

    /* 제곱 검증 */
    finite_a = rand()%0x1FFF;
    finite_c = finite_a;

	for(int i=0; i<=count; i++ )
	{
        finite_a = gf2m_square(finite_a);
        finite_c = gf2m_square_w_tab(finite_c, gf2m_tables->square_tab);
        if(finite_a!=finite_c)
            printf("error!!\n");
	}
    printf("제곱 체크 완료\n");

    /* 제곱근 검증 */
    finite_a = rand()%0x1FFF;
    finite_c = finite_a;

	for(int i=0; i<=count; i++ )
	{
        finite_a = gf2m_sqrt(finite_a);
        finite_c = gf2m_sqrt_w_tab(finite_c, gf2m_tables->sqrt_tab);
        if(finite_a!=finite_c)
            printf("error!!\n");
	}
    printf("제곱근 체크 완료\n");

    /* 역원 검증 */
    finite_a = rand()%0x1FFF;
    finite_c = finite_a;

	for(int i=0; i<=count; i++ )
	{
        finite_a = gf2m_inv(finite_a);
        finite_c = gf2m_inv_w_tab(finite_c, gf2m_tables->inv_tab);
        if(finite_a!=finite_c)
            printf("error!!\n");
    }
    printf("역원 체크 완료\n");


}