/**
 * SSX-Elfldr
 *
 * (C) 2021-2022 Lily/modeco80 <lily.modeco80@protonmail.ch>
 * under the terms of the MIT license.
 */

#include <stddef.h>

extern "C" size_t strlen(const char* __restrict str);

extern "C" {
void strrev(char* __restrict str) {
	int i;
	int j;
	unsigned char a;
	unsigned len = strlen(str);
	for(i = 0, j = len - 1; i < j; i++, j--) {
		a = str[i];
		str[i] = str[j];
		str[j] = a;
	}
}
}