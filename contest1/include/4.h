#include <stdint.h>
#include <stdbool.h>


extern int check_int(uint32_t u32) {
        /**
         * This constant shows power of two at which the "grinding" of the gap starts,
         * i.e. the precision of the integer part of the number is lost
         * Equal to the size of mantissa + 1
        **/
	const uint32_t MILLING_START_SHIFT = 24;	

        const uint32_t SHIFT_STEP = 1;

        /**
         * 1) ~(uint32_t)0 : 111...111
         * 2) ~(uint32_t)0 << MILLING_START_SHIFT : 111...111000...000 (24 zeroes in the right)
        **/
        uint32_t shift = ((~(uint32_t)0) << MILLING_START_SHIFT);

        uint32_t carryMask = (~(uint32_t)0);

        if (!(u32 & shift)) {
                return 1;
        }

        do {
                shift <<= SHIFT_STEP;
                carryMask <<= SHIFT_STEP;

		_Bool isInRightInterval = !(u32 & shift);
		_Bool isDivideByMillingPower = !(u32 & (~carryMask));

		if (isInRightInterval && isDivideByMillingPower) {
			return 1;
		}
        } while (shift);

	return 0;
}

