/**
 * SSX-Elfldr
 *
 * (C) 2021-2022 Lily/modeco80 <lily.modeco80@protonmail.ch>
 * under the terms of the MIT license.
 */

#include <stddef.h>
#include <stdint.h>

extern "C" char* itoa(int num, char* str, int base);
extern "C" size_t strlen(const char* __restrict str);

extern "C" {

int _vsnprintf(char* __restrict buf, int length, const char* __restrict format, __builtin_va_list va) {
	auto len = strlen(format);
	char itoa_buf[32];

	int buf_len = 0;

	for(int i = 0; i < len; ++i) {
		if(length != -1) // special sentinel value used by vsprintf as "don't care" value
			if(buf_len >= length)
				break;

		if(format[i] == '%') {
			switch(format[i + 1]) {
					// number
				case 'd':
				case 'i': {
					itoa(__builtin_va_arg(va, int), &itoa_buf[0], 10);
					int len = strlen(itoa_buf);
					for(int j = 0; j < len; ++j)
						buf[buf_len + j] = itoa_buf[j];

					buf_len += len;
					i++;
				} break;

					// TODO: 64bit numbers

				case 'X':
				case 'x': {
					auto vaarg = __builtin_va_arg(va, uint32_t);

					constexpr static const char* hex_digits_lower = "0123456789abcdef";
					constexpr static const char* hex_digits_upper = "0123456789ABCDEF";

					int digits = 0;
					for(uint32_t n = vaarg; n > 0; n >>= 4) {
						++digits;
					}

					if(digits == 0)
						digits = 1;

					buf[buf_len++] = '0';
					buf[buf_len++] = 'x';

					if(vaarg == 0) {
						buf[buf_len++] = '0';
					} else {
						auto shift_count = digits * 4;
						while(shift_count) {
							shift_count -= 4;

							if(format[i + 1] == 'x')
								buf[buf_len++] = hex_digits_lower[(vaarg >> shift_count) & 0x0f];
							else if(format[i + 1] == 'X')
								buf[buf_len++] = hex_digits_upper[(vaarg >> shift_count) & 0x0f];
						}
					}

					i++;
				} break;

				case 's': {
					constexpr const char* null_str = "(null)";
					const auto* c = __builtin_va_arg(va, char*);

					if(c == nullptr)
						c = null_str;

					int len = strlen(c);
					for(int j = 0; j < len; ++j)
						buf[buf_len + j] = c[j];

					buf_len += len;
					i++;
				} break;

					// output literal '%'
				case '%':
					buf[i] = '%';
					buf_len++;
					i += 2;
					break;

					// ignore unsupported formats
				default:
					buf[buf_len++] = format[i];
					break;
			}
		} else {
			buf[buf_len++] = format[i];
		}
	}

	// buf[buf_len] = '\0';
	return buf_len;
}

int vsprintf(char* __restrict buf, const char* __restrict format, __builtin_va_list va) {
	return _vsnprintf(buf, -1, format, va);
}

int vsnprintf(char* __restrict buf, int len, const char* __restrict format, __builtin_va_list va) {
	return _vsnprintf(buf, len, format, va);
}

// TODO: vsnprintf
}