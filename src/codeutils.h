// Fun code utilities.

#ifndef CODEUTILS_H
#define CODEUTILS_H

#include <cstring>
#include <cstdint>

namespace elfldr::util {
	
	/**
	 * The worst casting hack ever.
	 * It works, though.
	 */
	template<class Dest, class Source>
	constexpr Dest UBCast(Source source) {
		static_assert(sizeof(Dest) <= sizeof(Source), "Dest shouldn't be able to be the union's data carrier.");
		union {
			Source src;
			Dest dst;
		} u { .src = source };
		
		return u.dst;
	}
	
	// used to offset for classes i DO NOT feel like messing with
	// NO DATA MEMBERS. THIS SHOULD BE AN EMPTY BASECLASS.
	template<class T>
	struct IntrospectableType {
		
		template<class Y>
		constexpr Y FieldAtOffset(std::size_t offset) {
			return static_cast<Y>(static_cast<T*>(this) + offset);
		}
		
	};
	
	/**
	 * Vtable pointer structure for GCC.
	 */
	struct GnuVtablePointer {
		std::uint16_t adj_upper;
		std::uint16_t adj_lower;
		
		/**
		 * True function pointer.
		 */
		void* function_ptr;
		
		template<auto Func>
		constexpr void assign_function() {
			// ?
			static_assert(sizeof(Func) == sizeof(void*), "Don't put in an virtual pmf...");
			function_ptr = UBCast<void*>(Func);
		}
		
		// TODO: operator() for call?
	};

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
		// TODO: static_assert for dword-alignment
		memset(start, 0x0, N * sizeof(uintptr_t));
	}

// TODO: inline+forceinline?
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
	
	constexpr void* Ptr(uintptr_t address) {
		return UBCast<void*>(address);
	}
	
	/**
	 * Call a function at the given address,
	 * with the given arguments.
	 *
	 * \tparam Ret Return type.
	 * \tparam Args Varadic arg types.
	 *
	 * \param[in] ptr Pointer to function. Can use util::Ptr().
	 * \param[in] args Argument pack to forward to function.
	 */
	template<class Ret, class... Args>
	constexpr Ret CallFunction(void* ptr, Args... args) {
		using FuncT = Ret(*)(Args...);
		return (UBCast<FuncT>(ptr))(args...);
	}
		
} // namespace util

#endif // CODEUTILS_H