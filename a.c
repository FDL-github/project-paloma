#include "bench.h"

int main()
{
    /* run paloma and check */
    paloma_check();

    /* benchmark */
    paloma_cycle_print();
    // paloma_time_print();

    return 0;
}