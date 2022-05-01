/**
 * SSX-Elfldr
 *
 * (C) 2021-2022 Lily/modeco80 <lily.modeco80@protonmail.ch>
 * under the terms of the MIT license.
 */

extern "C" {

int vsprintf(char* __restrict buf, const char* __restrict format, __builtin_va_list va);

int sprintf(char* __restrict buf, const char* __restrict format, ...) {
	__builtin_va_list val;
	__builtin_va_start(val, format);
	int res = vsprintf(buf, format, val);
	__builtin_va_end(val);
	return res;
}
}
