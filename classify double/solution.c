#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>



/**
 * Library-level functions.
 * You should use them in the main sections.
 */

uint64_t convertToUint64 (double number) {
    return *((uint64_t *)(&number));
}

bool getBit(const uint64_t number, const uint8_t index) {
	return (bool)((((uint64_t)1 << index) & number) >> index);
}


/**
 * Checkers here:
 */

bool checkForPlusZero(uint64_t number) {
	return number == 0;
}

bool checkForMinusZero (uint64_t number) {
    return number == 0x8000000000000000;
}

bool checkForPlusInf(uint64_t number) {
	bool temp = 1;
	for (int i = 52; i <= 62; i++) {
		temp = temp && getBit(number, i);
	}
	for (int i = 0; i <= 51; i++) {
		temp = temp && !getBit(number, i);
	}
	return temp && !getBit(number, 63);
}

bool checkForMinusInf(uint64_t number) {
	return checkForPlusInf(number << 1 >> 1) && getBit(number, 63);
}

bool checkForPlusNormal(uint64_t number) {
	int temp = 0;
	for (int i = 52; i <= 62; i++)
		temp += getBit(number, i);
	if (temp < 11 && temp >= 1)
		return 1 && !getBit(number, 63);
	return 0;
}

bool checkForMinusNormal(uint64_t number) {
	return checkForPlusNormal(number << 1 >> 1) && getBit(number, 63);
}

bool checkForPlusDenormal(uint64_t number) {
	bool temp_for_fract = 0;
	bool temp_for_exp = 1;
	for (int i = 0; i <= 51; i++) {
		temp_for_fract = temp_for_fract || getBit(number, i);
	}
	for (int i = 52; i <= 62; i++) {
		temp_for_exp = temp_for_exp && !getBit(number, i);
	}
	if (temp_for_fract && temp_for_exp)
		return 1 && !getBit(number, 63);
	return 0;
}

bool checkForMinusDenormal(uint64_t number) {
	return checkForPlusDenormal(number << 1 >> 1) && getBit(number, 63);
}

bool checkForSignalingNan(uint64_t number) {
	bool temp_for_exp = 1;
	bool temp_for_fract = 0;
	for (int i = 0; i <= 50; i++) {
		temp_for_fract = temp_for_fract || getBit(number, i);
	}
	for (int i = 52; i <= 62; i++) {
		temp_for_exp = temp_for_exp && getBit(number, i);
	}
	if (temp_for_fract && !getBit(number, 51) && temp_for_exp)
		return 1;
	return 0;
}

bool checkForQuietNan(uint64_t number) {
	bool temp_for_exp = 1;
	for (int i = 52; i <= 62; i++) {
		temp_for_exp = temp_for_exp && getBit(number, i);
	}
	if (temp_for_exp && getBit(number, 51))
		return 1;
	return 0;
}


void classify (double number) {
    if (checkForPlusZero(convertToUint64(number))) {
        printf("Plus zero\n");
    }

    else if (checkForMinusZero(convertToUint64(number))) {
        printf("Minus zero\n");
    }

    else if (checkForPlusInf(convertToUint64(number))) {
        printf("Plus inf\n");
    }

    else if (checkForMinusInf(convertToUint64(number))) {
        printf("Minus inf\n");
    }

    else if (checkForPlusNormal(convertToUint64(number))) {
        printf("Plus normal\n");
    }

    else if (checkForMinusNormal(convertToUint64(number))) {
        printf("Minus normal\n");
    }

    else if (checkForPlusDenormal(convertToUint64(number))) {
        printf("Plus Denormal\n");
    }

    else if (checkForMinusDenormal(convertToUint64(number))) {
        printf("Minus Denormal\n");
    }

    else if (checkForSignalingNan(convertToUint64(number))) {
        printf("Signailing NaN\n");
    }

    else if (checkForQuietNan(convertToUint64(number))) {
        printf("Quiet NaN\n");
    }

    else {
        printf("Error.\n");
    }
}
