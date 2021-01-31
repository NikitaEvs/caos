#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>


double calculate(int32_t argc, char **argv) {
	static const int32_t 	BASE16 = 16,
	      		     	BASE27 = 27;

	double 		x = 0;
	uint64_t 	y = 0, 
			z = 0;
	char 		inputHex[32];

	scanf("%lf %s", &x, inputHex);

	char* endHex = inputHex + 31;
	char* end27Base = argv[1] + strlen(argv[1]);
	y = strtol(inputHex, &(endHex), BASE16);

	if (argc > 1) {
		z = strtol(argv[1], &(end27Base), BASE27);
	}

	return x + y + z;
}

int main(int32_t argc, char **argv) {
	printf("%.3f \n", calculate(argc, argv));

	return 0;
}
