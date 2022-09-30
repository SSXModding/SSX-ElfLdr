/**
 * SSX-Elfldr
 *
 * (C) 2021-2022 Lily/modeco80 <lily.modeco80@protonmail.ch>
 * under the terms of the MIT license.
 */

// Fun code utilities, provided by the "libutils" library.

#ifndef CODEUTILS_H
#define CODEUTILS_H

#include <mlstd/Assert.h>
#include <mlstd/Utility.h>
#include <stdint.h>
#include <string.h>

#include <utils/Utils.h>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wstringop-overflow="

namespace elfldr::util {

	/**
	 * CRTP baseclass for empty classes,
	 * to still allow introspection of any field
	 */
	template <class T>
	struct IntrospectableType {
		template <class Y>
		constexpr Y FieldAtOffset(size_t offset) {
			return static_cast<Y>(static_cast<T*>(this) + offset);
		}
	};

	/**
	 * VTable pointer structure for GCC2 C++ ABI VTables.
	 * Allows looking up things
	 */
	struct GnuVtablePointer {
		// Looking at generated assembly reveals
		// that these are actually emitted as 2 shorts.
		uint16_t adj_upper; // TODO: This is probably vcall offset (see IABI, which is pretty much GCC2 ABI but standardized)
		uint16_t adj_lower; // TODO: This is probably vbase offset (see IABI)

		/**
		 * The function pointer.
		 */
		void* function_ptr;

		template <auto Func>
		constexpr void assign_function() {
			// ?
			static_assert(sizeof(Func) == sizeof(void*), "Don't put in a virtual PMF...");
			function_ptr = reinterpret_cast<void*>(Func);
		}

		// I think? this isn't going to be used worth a damn yet
		template <class Res = void, class... Args>
		inline Res operator()(void* pObj, Args&&... args) const {
			MLSTD_ASSERT(function_ptr != nullptr);
			return CallFunction<Res>(function_ptr, reinterpret_cast<uintptr_t>(pObj) + adj_upper, Forward<Args>(args)...);
		}
	};

	/**
	 * Replace an existing string with a new string.
	 *
	 * \param[out] addr Address of string to replace.
	 * \param[in] string String to replace.
	 */
	void ReplaceString(void* addr, const char* string);

	/**
	 * Write a string at an address.
	 *
	 * \param[out] addr Address of where to write.
	 * \param[in] string String to write.
	 */
	void WriteString(void* addr, const char* string);

	constexpr bool IsPowOf2(uint32_t val) {
		return !((val != 0) && ((val & (val - 1))));
	}

	template <uint32_t alignment>
	constexpr bool IsAlignedNBytes(const void* __restrict address) {
		static_assert(IsPowOf2(alignment), "Alignment must be a power of 2, silly!!");
		return !((uintptr_t)address & (alignment - 1));
	}

	/**
	 * Check if the passed-in address is Double-word aligned,
	 * as MIPS instructions need to be DWord-aligned.
	 *
	 * \param[in] addr The address to check.
	 */
	constexpr bool IsInstructionAligned(const void* __restrict addr) {
		return IsAlignedNBytes<4>(addr);
	}

	/**
	 * Fill an aligned section with MIPS nop (all zeros.)
	 *
	 * \tparam N Instruction count
	 * \param[in] start Start address.
	 */
	template <size_t N>
	constexpr void NopFill(void* start) {
		MLSTD_ASSERT(IsInstructionAligned(start));
		memset(start, 0x0, N * sizeof(uint32_t));
	}

	/**
	 * Make a pointer from a uintptr_t.
	 *
	 * \param[in] address Address.
	 * \return void* pointer
	 */
	inline void* Ptr(uintptr_t address) {
		return reinterpret_cast<void*>(address);
	}

	/**
	 * Dereference address as T.
	 *
	 * \param[in] addr Address
	 * \tparam T type.
	 */
	template <class T>
	constexpr T& MemRefTo(void* addr) {
		return *reinterpret_cast<T*>(addr);
	}

	/**
	 * Call a function at the given address,
	 * with the given arguments, and the return type.
	 *
	 * Turns directly into a call to the given subroutine,
	 * using a register. Pretty cool, huh?
	 *
	 * \tparam Ret Return type. Defaults to "void".
	 * \tparam Args Varadic arg types.
	 *
	 * \param[in] ptr Pointer to function. Can use util::Ptr().
	 * \param[in] args Argument pack to forward to function as arguments.
	 */
	template <class Ret = void, class... Args>
	constexpr Ret CallFunction(void* ptr, Args... args) {
		return (reinterpret_cast<Ret(*)(Args...)>(ptr))(args...);
	}

	/**
	 * A basic boilerplate function wrapper object,
	 * making it possible for relatively zero-maintainance,
	 * negative cost function wrappers to be generated..
	 * at compile time!
	 *
	 * And it's all thanks to CallFunction<Ret>().
	 *
	 * \tparam FunctionAddress Function address.
	 *
	 * \tparam IsVaradic True if the function takes varadic arguments.
	 *					 If true, overload resolution for a version
	 * 					 of operator() will be permitted, which itself
	 *					 permits arbitrary additional varadic arguments.
	 *
	 * \tparam Ret The return type of the wrapped function.
	 * \tparam ArgTypes All argument types.
	 */
	template <bool IsVaradic, class Ret, class... BaseArgTypes>
	struct BasicFunctionWrapper {
		// TODO: perfect forwarding

		/**
		 * Call operator, for non-varadic functions.
		 * Only takes base argument types.
		 */
		constexpr Ret operator()(BaseArgTypes... args) const {
			//DebugOut("address is 0x%08x", functionAddress);
			return CallFunction<Ret>(Ptr(functionAddress), args...);
		}

		/**
		 * Call operator, for varadic functions.
		 * This overload only participates in overload resolution
		 * if this function wrapper is varadic.
		 *
		 * \tparam AnyVarArgs Any varadic arguments to pass to the function.
		 */
		template <class... VarArgTypes>
		requires(IsVaradic) constexpr Ret operator()(BaseArgTypes... args, VarArgTypes... varargs) const {
			return CallFunction<Ret>(Ptr(functionAddress), args..., varargs...);
		}

		constexpr void SetFunctionAddress(uintptr_t addr) {
			functionAddress = addr;
		}

	   private:
		uintptr_t functionAddress;
	};

	/**
	 * A wrapper for a varadic function (i.e: printf)
	 */
	template <class Ret, class... ArgTypes>
	using VarFunction = BasicFunctionWrapper<true, Ret, ArgTypes...>;

	/**
	 * A wrapper for a regular function.
	 */
	template<class Ret, class... ArgTypes>
	using Function = BasicFunctionWrapper<false, Ret, ArgTypes...>;

} // namespace elfldr::util


#pragma GCC diagnostic pop
#endif // CODEUTILS_H