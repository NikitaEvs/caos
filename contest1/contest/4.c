#include <stdint.h>
#include <stdbool.h>



extern int check_int(uint32_t u32) {
        /**
         * This constant shows power of two at which the "grinding" of the gap starts,
         * i.e. the precision of the integer part of the number is lost
         * Equal to the size of mantissa + 1
        **/
	const uint32_t MILLING_START_SHIFT = 24;	
	/**
         * Maximum bias for checking the "grinding" because the type is
	 * uint32_t
	**/
	const uint32_t MILLING_FINISH_SHIFT = 32; 

        uint32_t shift = 0;
	for(; MILLING_START_SHIFT + shift <= MILLING_FINISH_SHIFT; ++shift) {
		_Bool isInRightInterval = true;
		_Bool isDivideByMillingPower = true;
		
		if (MILLING_START_SHIFT + shift < MILLING_FINISH_SHIFT) {		
			isInRightInterval = (u32 < ((uint32_t)1 << (MILLING_START_SHIFT + shift)));
		}

		if (shift > 0) {
			uint32_t MASK_REMAINDER = ((uint32_t)1 << shift) - 1;
			isDivideByMillingPower = ((u32 & MASK_REMAINDER) == 0);	
		}

		if (isInRightInterval && isDivideByMillingPower) {
			return 1;
		}
	}
	return 0;
}
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
