#include "4.h"
#include <stdio.h>

extern int
check_int_test(uint32_t u32)
{
    if (u32>>24 == 0)
    {
        return 1;
    }
    uint32_t start_mask = (uint32_t)1<<31;
    uint32_t ending_mask = (1<<8) - 1;
    for (int32_t i = 0; i < 8; ++i)
    {
        if ((start_mask & u32) != 0)
        {
            return (u32 & ending_mask) == 0;
        } else
        {
            start_mask>>=1;
            ending_mask>>=1;
        }
    }
    return 0;
}

int main() {
        for(uint32_t i = 0; i < UINT32_MAX - 1; ++i) {
                if (i % 10000000 == 0) {
                        printf("%u\n", i);
                }
                if (check_int_test(i) != check_int(i)) {
                        printf("%s", "kek");
                        printf("%u\n", i);
                        return 0;
                }
        }


	return 0;
}
