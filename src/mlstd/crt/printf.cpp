/**
 * SSX-Elfldr
 *
 * (C) 2021-2022 Lily/modeco80 <lily.modeco80@protonmail.ch>
 * under the terms of the MIT license.
 */

#include <sio.h>
#include <stdarg.h>
#include <stdio.h>

extern "C" {
// int vsnprintf(char* __restrict buf, int len, const char* __restrict format, va_list va);

int mlstd_vprintf(const char* __restrict format, va_list vs) {
	static char buffer[2048] {};
	int res = vsnprintf(buffer, sizeof(buffer), format, vs);
	sio_putsn(buffer);
	return res;
}

int mlstd_printf(const char* __restrict format, ...) {
	__builtin_va_list val;
	__builtin_va_start(val, format);

	int res = mlstd_vprintf(format, val);

	__builtin_va_end(val);
	return res;
}

int mlstd_puts(const char* __restrict string) {
	return sio_puts(string);
}
}