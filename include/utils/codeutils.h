/**
 * SSX-Elfldr
 *
 * (C) 2021-2022 Lily/modeco80 <lily.modeco80@protonmail.ch>
 * under the terms of the MIT license.
 */

// Fun code utilities, provided by the "libutils" library.

#ifndef CODEUTILS_H
#define CODEUTILS_H

#include <cstdint>
#include <cstring>

// needs util::UBCast<Dest, Source>()
#include <utils/utils.h>

namespace elfldr::util {

	/**
	 * CRTP baseclass for empty classes,
	 * to still allow introspection of any field
	 */
	template <class T>
	struct IntrospectableType {
		template <class Y>
		constexpr Y FieldAtOffset(std::size_t offset) {
			return static_cast<Y>(static_cast<T*>(this) + offset);
		}
	};

	/**
	 * Vtable pointer structure for GCC 2 Abi.
	 */
	struct GnuVtablePointer {
		// Looking at generated assembly reveals
		// that these are actually emitted as 2 shorts.
		std::uint16_t adj_upper;
		std::uint16_t adj_lower;

		/**
		 * The function pointer.
		 */
		void* function_ptr;

		template <auto Func>
		constexpr void assign_function() {
			// ?
			static_assert(sizeof(Func) == sizeof(void*), "Don't put in a virtual PMF...");
			function_ptr = UBCast<void*>(Func);
		}

		// TODO: varadic constexpr operator() for call?
		// could be done with util::CallFunction().. although I don't really see the point
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

	/**
	 * Check if the passed-in address is word aligned,
	 * as MIPS instructions need to be word-aligned.
	 *
	 * \param[in] addr The address to check.
	 */
	bool IsInstructionAligned(void* addr);

	/**
	 * Writes the 4 instructions pointed to by code to something like:
	 * \code
	 *  lui s0, 0x0018 ; top 16 bits
	 *  ori s0, s0, 0xBADE ; lower 16 bits
	 *  jalr s0
	 *  nop ; branch delay
	 * \code
	 *
	 * The given subroutine is called with no arguments.
	 * The written code also assumes `s0` is a safe register (i.e: it is saved during the hook.).
	 *
	 * \param[out] Code address. 4 instructions space MUST be valid to write to.
	 * \param[in] subroutine The subroutine address to call.
	 */
	void WriteRelocatableCall0(void* code, void* subroutine);

	/**
	 * Fill an aligned section with MIPS nop (all zeros.)
	 *
	 * \tparam N Instruction count
	 * \param[in] start Start address.
	 */
	template <size_t N>
	constexpr void NopFill(void* start) {
		// ELFLDR_VERIFY(IsInstructionAligned(start));
		memset(start, 0x0, N * sizeof(std::uint32_t));
	}

	/**
	 * Make a pointer from a uintptr_t.
	 *
	 * \param[in] address Address.
	 * \return void* pointer
	 */
	constexpr void* Ptr(std::uintptr_t address) {
		return UBCast<void*>(address);
	}

	/**
	 * Dereference address as T.
	 *
	 * \param[in] addr Address
	 * \tparam T type.
	 */
	template <class T>
	constexpr T& MemRefTo(void* addr) {
		return *UBCast<T*>(addr);
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
		using FuncT = Ret (*)(Args...);
		return (UBCast<FuncT>(ptr))(args...);
	}

	// TODO: Should there be a runtime version of this which stores an address
	//  or pair of addresses depending on game/release/region?

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
	template <uintptr_t FunctionAddress, bool IsVaradic, class Ret, class... BaseArgTypes>
	struct BasicFunctionWrappa {
		/**
		 * Call operator, for non-varadic functions.
		 * Only takes base argument types.
		 */
		constexpr Ret operator()(BaseArgTypes... args) const {
			return CallFunction<Ret>(Ptr(FunctionAddress), args...);
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
			return CallFunction<Ret>(Ptr(FunctionAddress), args..., varargs...);
		}
	};

	/**
	 * A wrapper for a varadic function (i.e: printf)
	 */
	template <uintptr_t FunctionAddress, class Ret, class... ArgTypes>
	using VarFunction = BasicFunctionWrappa<FunctionAddress, true, Ret, ArgTypes...>;

	/**
	 * A wrapper for a regular function.
	 */
	template <uintptr_t FunctionAddress, class Ret, class... ArgTypes>
	using Function = BasicFunctionWrappa<FunctionAddress, false, Ret, ArgTypes...>;

} // namespace elfldr::util

#endif // CODEUTILS_H