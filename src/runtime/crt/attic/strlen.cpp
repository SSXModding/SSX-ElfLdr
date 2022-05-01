
#include <stddef.h>
extern "C" {
size_t strlen(const char* __restrict str) {
	const char* s = str;

	// Iterate through the string until NUL character
	while(*s)
		++s;

	return (s - str);
}
}