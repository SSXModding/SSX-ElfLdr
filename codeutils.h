// fun code utilities
#ifndef CODEUTILS_H
#define CODEUTILS_H

#include <cstring>
#include <cstdio>

namespace util {
	
	// write a message with a little prefix.
	template<class... T>
	constexpr void DebugOut(const char* fmt, T... t) {
		char buf[256]{};
 
		// this is PAIN.
		memcpy(&buf[0], "[ElfLdr] ", sizeof("[ElfLdr]"));
		snprintf(&buf[sizeof("[ElfLdr]")], (sizeof(buf) - sizeof("[ElfLdr]") - 1), fmt, t...);
		puts(buf);
	}

	void ReplaceString(void* addr, const char* string);

	// replacestring but cooler.
	void WriteString(void* addr, const char* string);

	// fill an area with mips NOP
	// N is the number of instructions to write.
	template<size_t N>
	constexpr void NopFill(void* start) {
		memset(start, 0x0, N * sizeof(uintptr_t));
	}

	// dereference address as T
	template<class T>
	constexpr T& MemRefTo(void* addr) {
		return *static_cast<T*>(addr);
	}
		
} // namespace util

#endif // CODEUTILS_H