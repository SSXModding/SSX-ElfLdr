/**
 * SSX-Elfldr
 *
 * (C) 2021-2022 Lily/modeco80 <lily.modeco80@protonmail.ch>
 * under the terms of the MIT license.
 */

#include <stddef.h>


extern "C" size_t strlen(const char* __restrict str);
extern "C" void strrev(char* __restrict str);


extern "C" {


char* itoa(int num, char* str, int base) {
	int i = 0;
	bool isNegative = false;

	/* Handle 0 explicitly, otherwise empty string is printed for 0 */
	if(num == 0) {
		str[i++] = '0';
		str[i] = '\0';
		return str;
	}

	// In standard itoa(), negative numbers are handled only with
	// base 10. Otherwise numbers are considered unsigned.
	if(num < 0 && base == 10) {
		isNegative = true;
		num = -num;
	}

	// Process individual digits
	while(num != 0) {
		int rem = num % base;
		str[i++] = (rem > 9) ? (rem - 10) + 'a' : rem + '0';
		num = num / base;
	}

	// If number is negative, append '-'
	if(isNegative)
		str[i++] = '-';

	str[i] = '\0'; // Append string terminator
	strrev(str);
	return str;
}
}