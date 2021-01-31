

int main() {
	process();

	return 0;
}
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

enum {
	FIRST_LOWERCASE_LETTER 		= 'a',
	FIRST_CAPITAL_LETTER 		= 'A',
	FIRST_NUMBER 			= '0',
	FIRST_LOWERCASE_LETTER_BIT 	= 36,
	FIRST_CAPITAL_LETTER_BIT 	= 10,
	ALPHABET_SIZE 			= 62
};

typedef struct {
	uint64_t result;
	uint64_t current;
} set;

char getSymbol(uint8_t bitBias) {
	if (bitBias >= FIRST_LOWERCASE_LETTER_BIT) {
		return FIRST_LOWERCASE_LETTER + (bitBias - FIRST_LOWERCASE_LETTER_BIT);
	} else if (bitBias >= FIRST_CAPITAL_LETTER_BIT) {
		return FIRST_CAPITAL_LETTER + (bitBias - FIRST_CAPITAL_LETTER_BIT);
	} else {
		return FIRST_NUMBER + bitBias;
	}
}

uint8_t getBias(char symbol) {
	if (symbol >= FIRST_LOWERCASE_LETTER) {
		return FIRST_LOWERCASE_LETTER_BIT + (symbol - FIRST_LOWERCASE_LETTER);
	} else if (symbol >= FIRST_CAPITAL_LETTER) {
		return FIRST_CAPITAL_LETTER_BIT + (symbol - FIRST_CAPITAL_LETTER);
	} else {
		return symbol - FIRST_NUMBER;
	}
}

set* setConstructor() {
	set* mainSet = (set*) malloc(sizeof(set));

	mainSet->result = 0;
	mainSet->current = 0;

	return mainSet;
}

void setWrite(set* mainSet, char buffer) {
	mainSet->current |= ((uint64_t)1 << getBias(buffer));
}

void setRead(set* mainSet, char (*buffer)[ALPHABET_SIZE]) {
	uint8_t pointer = 0; // pointer to the writing position in buffer
        uint8_t bias = 0;
	for (; bias < ALPHABET_SIZE; ++bias) {
		uint64_t mask = ((uint64_t)1 << bias);
		if ((mainSet->result & mask) == mask) {
			(*buffer)[pointer] = getSymbol(bias); 
			++pointer;
		}
	}
	(*buffer)[pointer] = '\0';
}

void setDestructor(set* mainSet) {
	free(mainSet);
}

void setIntersection(set* mainSet) {
	mainSet->result &= mainSet->current;
	mainSet->current = 0;
}

void setUnion(set* mainSet) {
	mainSet->result |= mainSet->current;
	mainSet->current = 0;
}

void setDisjunctiveUnion(set* mainSet) {
	mainSet->result ^= mainSet->current;
	mainSet->current = 0;
}

void setComplement(set* mainSet) {
	mainSet->result = ~mainSet->result;
}

void handle(set* mainSet, char buffer) {
	if (buffer == '&') {
		setIntersection(mainSet);	
	} else if (buffer == '|') {
		setUnion(mainSet);
	} else if (buffer == '^') {
		setDisjunctiveUnion(mainSet);
	} else if (buffer == '~') {
		setComplement(mainSet);
	} else if ((buffer >= '0') && (buffer <= 'z')) {
		setWrite(mainSet, buffer);		
	}
}

void process() {
	set* mainSet = setConstructor();

	char buffer = 0;

	do {
		buffer = fgetc(stdin);

		if ((buffer == EOF) || (buffer == '\n')) {
			break;
		} else {
			handle(mainSet, buffer);
		}
	} while (buffer != EOF);

	char outputBuffer[ALPHABET_SIZE] = {0};
	setRead(mainSet, &outputBuffer);
	printf("%s\n", outputBuffer);

	setDestructor(mainSet);
}
