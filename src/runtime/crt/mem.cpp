/**
 * SSX-Elfldr
 *
 * (C) 2021-2022 Lily/modeco80 <lily.modeco80@protonmail.ch>
 * under the terms of the MIT license.
 */

#include <stddef.h>
#include <stdint.h>

extern "C" {

void* memcpy(void* __restrict dest_ptr, const void* __restrict src_ptr, size_t n) {
	uint8_t* pd = (uint8_t*)dest_ptr;
	uint8_t const* ps = (uint8_t const*)src_ptr;
	for(; n--;)
		*pd++ = *ps++;
	return dest_ptr;
}
}