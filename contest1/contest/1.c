uint16_t satsum(uint16_t x, uint16_t y) {
	uint16_t result = x + y;
	
	if (result < x) {
		result = ~(uint16_t)0;
	}

	return result;
}

#include <stdio.h>
#include "1.h"

int main() {
	printf("%d\n", satsum(20000, 20000));
}
