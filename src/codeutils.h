// Fun code utilities.

#ifndef CODEUTILS_H
#define CODEUTILS_H

#include <cstring>
#include <cstdint>

namespace elfldr::util {

	void ReplaceString(void* addr, const char* string);

	// replacestring but cooler.
	void WriteString(void* addr, const char* string);

	/**
	 * Fill an aligned section with MIPS nop (all zeros.)
	 *
	 * \tparam N Instruction count
	 * \param[in] start Start.
	 */
	template<size_t N>
	constexpr void NopFill(void* start) {
		memset(start, 0x0, N * sizeof(uintptr_t));
	}

	/**
	 * Dereference address as T.
	 *
	 * \param[in] addr Address
	 * \tparam T type.
	 */
	template<class T>
	constexpr T& MemRefTo(void* addr) {
		return *static_cast<T*>(addr);
	}
		
} // namespace util

#endif // CODEUTILS_H