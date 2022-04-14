/**
 * SSX-Elfldr
 *
 * (C) 2021-2022 Lily/modeco80 <lily.modeco80@protonmail.ch>
 * under the terms of the MIT license.
 */

#include <stdint.h>

extern "C" {

int strcmp(const char* __restrict s1, const char* __restrict s2) {
	if(s1 == s2)
		return 0;

	while(*s1 == *s2++)
		if(*s1++ == 0)
			return 0;

	return (*(const uint8_t*)s1 - *(const uint8_t*)(s2 - 1));
}

char* strcat(char* dest, const char* src) {
	char* rdest = dest;

	// advance to null terminator
	while(*dest)
		dest++;

	while((*dest++ = *src++))
		;
	return rdest;
}

}