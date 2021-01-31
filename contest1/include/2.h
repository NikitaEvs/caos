#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

typedef enum {
    PlusZero      = 0x00,
    MinusZero     = 0x01,
    PlusInf       = 0xF0,
    MinusInf      = 0xF1,
    PlusRegular   = 0x10,
    MinusRegular  = 0x11,
    PlusDenormal  = 0x20,
    MinusDenormal = 0x21,
    SignalingNaN  = 0x30,
    QuietNaN      = 0x31
} float_class_t;

typedef enum { 
        EXPONENT_MASK_LEFT_SHIFT = 52,
        SIGN_MASK_LEFT_SHIFT = 63,
        MANTISSA_RIGHT_SHIFT = 12,
        MANTISSA1_LEFT_SHIFT = 51,
        /**
         * 1) ~(uint64_t)0 : 111...111
         * 2) ~(uint64_t)0 << 52 : 111...111000...000 (52 zeros - size of the mantissa)
         * 3) (uint64_t)1 << 63 : 1000...000
         * 4) (~(uint64_t)0 << 52) ^ ((uint64_t)1 << 63) : 0111...111000...000
        **/
        EXPONENT_MASK = (~(uint64_t)0 << EXPONENT_MASK_LEFT_SHIFT) ^ ((uint64_t)1 << SIGN_MASK_LEFT_SHIFT),
        /**
         * 1) ~(uint64_t)0 : 111...111
         * 2) ~(uint64_t)0 >> 12 : 000...000111...111
        **/ 
        MANTISSA_MASK = ~(uint64_t)0 >> MANTISSA_RIGHT_SHIFT,
        SIGN_MASK = (uint64_t)1 << SIGN_MASK_LEFT_SHIFT,
        MANTISSA1_MASK = (uint64_t)1 << MANTISSA1_LEFT_SHIFT
} MASKS;


uint64_t* decimalCast(double* input) {
	void* inputRaw = input;
	return (uint64_t*)inputRaw;	
}

inline static uint64_t getExponent(uint64_t *input) {
	return (*input & EXPONENT_MASK); 
}

inline static uint64_t getMantissa(uint64_t *input) {
	return (*input & MANTISSA_MASK);
}

inline static uint64_t getSign(uint64_t *input) {
	return (*input & SIGN_MASK); 
}

inline static _Bool isExponentAll1(uint64_t *input) {
	return (_Bool)(getExponent(input) == EXPONENT_MASK);
}

inline static _Bool isExponentAll0(uint64_t *input) {
	return (_Bool)(getExponent(input) == (uint64_t)0);
}

inline static _Bool isMantissaAll0(uint64_t *input) {
	return (_Bool)(getMantissa(input) == (uint64_t)0);
}

inline static _Bool isMantissaAll1(uint64_t *input) {
	return (_Bool)(getMantissa(input) == MANTISSA_MASK);
}

inline static _Bool isSign1(uint64_t *input) {
	return (_Bool)(getSign(input) == SIGN_MASK);
}

inline static _Bool isFirstSignOfMantissa1(uint64_t *input) {
	/**
	 * Bit mask: 000...0001000...000 (51 zeros in the end)
	**/
	return (_Bool)((*input & MANTISSA1_MASK) == MANTISSA1_MASK);	
}

extern float_class_t classify(double *value_ptr) {
	uint64_t* decimalPtr = decimalCast(value_ptr);		

	if (isExponentAll0(decimalPtr)) {
		if (isMantissaAll0(decimalPtr)) {
			if (isSign1(decimalPtr)) {
				return MinusZero; 
			} else {
				return PlusZero;
			}
		} else {
			if (isSign1(decimalPtr)) {
				return MinusDenormal;
			} else {
				return PlusDenormal;
			}
		}
	} else {
		if (isExponentAll1(decimalPtr)) {
			if (isMantissaAll0(decimalPtr)) {
				if (isSign1(decimalPtr)) {
					return MinusInf;
				} else {
					return PlusInf;
				}
			} else {
				if (isFirstSignOfMantissa1(decimalPtr)) {
					return QuietNaN;
				} else {
					return SignalingNaN;
				}
			}
		} else {
			if (isSign1(decimalPtr)) {
				return MinusRegular;
			} else {
				return PlusRegular;
			}
		}
	} 
}
