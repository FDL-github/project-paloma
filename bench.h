#ifndef BENCH_H
#define BENCH_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <time.h>

#include "api.h"

#define OFF 0
#define ON 1

#ifndef BENCH_MODE
#define BENCH_MODE OFF
#endif

/********************/
/* benchmark: cycle */
/********************/

typedef struct
{
    uint64_t gen_rand_goppa_code;
    uint64_t get_scrambled_code;
    uint64_t gen_key;
    uint64_t encrypt;
    uint64_t encap;
    uint64_t construct_key_equ;
    uint64_t solve_key_equ;
    uint64_t find_err_vec;
    uint64_t decrypt;
    uint64_t decap;
} PalomaCycleResult;

extern uint64_t cycle_start;
extern uint64_t cycle_end;
extern PalomaCycleResult paloma_cycle_result;

/*******************/
/* benchmark: time */ //! 시간으로 잴 수도 있으니까 일단 만들어놓음.
/*******************/
typedef struct
{
    double gen_rand_goppa_code;
    double get_scrambled_code;
    double gen_key;
    double encrypt;
    double encap;
    double construct_key_equ;
    double solve_key_equ;
    double find_err_vec;
    double decrypt;
    double decap;
} PalomaTimeResult;

extern double time_start;
extern double time_end;
extern PalomaTimeResult paloma_time_result;

static inline uint64_t cycle_read()
{
    uint64_t result;
    asm volatile("mrs %0, cntvct_el0" : "=r"(result));
    return result;
}

static inline double time_read()
{
    return clock();
}

void paloma_check();

void paloma_cycle_print();

// paloma_time_print();

#endif