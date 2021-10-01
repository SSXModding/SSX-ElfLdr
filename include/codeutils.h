// Fun code utilities, provided by the "libutils" libtary.

#ifndef CODEUTILS_H
#define CODEUTILS_H

#include <cstring>
#include <cstdint>

// needs util::UBCast<Dest, Source>()
#include <utils.h>

namespace elfldr::util {
	
	/**
	 * CRTP baseclass for empty classes,
	 * to still allow introspection of any field
	 */
	template<class T>
	struct IntrospectableType {
		
		template<class Y>
		constexpr Y FieldAtOffset(std::size_t offset) {
			return static_cast<Y>(static_cast<T*>(this) + offset);
		}
		
	};
	
	/**
	 * Vtable pointer structure for GCC 2 Abi.
	 */
	struct GnuVtablePointer {
		// Looking at generated assembly reveals
		// that these are actually 2 shorts.
		std::uint16_t adj_upper;
		std::uint16_t adj_lower;
		
		/**
		 * The function pointer.
		 */
		void* function_ptr;
		
		template<auto Func>
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
	 * Fill an aligned section with MIPS nop (all zeros.)
	 *
	 * \tparam N Instruction count
	 * \param[in] start Start address.
	 */
	template<size_t N>
	constexpr void NopFill(void* start) {
		// TODO: static_assert for dword-alignment?
		// if this is implemented,
		// it should be a common "constexpr bool IsPtrInstructionAligned(const void* ptr)" thing
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
	template<class T>
	constexpr T& MemRefTo(void* addr) {
		return *UBCast<T*>(addr);
	}
	
	/**
	 * Call a function at the given address,
	 * with the given arguments.
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
	template<class Ret = void, class... Args>
	constexpr Ret CallFunction(void* ptr, Args... args) {
		using FuncT = Ret(*)(Args...);
		return (UBCast<FuncT>(ptr))(args...);
	}
	
	/**
	 * A basic boilerplate function wrapper object,
	 * making it possible for relatively zero-maintainance
	 * function wrappers to be generated.. at compile time!
	 *
	 * And it's all thanks to CallFunction<Ret>().
	 *
	 * \tparam Ret Return type.
	 * \tparam FunctionAddress Function address.
	 * \tparam IsVaradic True if the function takes varadic arguments.
	 *					 This will enable another overload to operator()
	 *					 which permits additional varadic arguments.
	 *
	 * \tparam ArgTypes Base argument types.
	 */
	template<class Ret, uintptr_t FunctionAddress, bool IsVaradic, class ...ArgTypes>
	struct FunctionWrappa {
		
		/**
		 * Call operator, for regular functions.
		 */
		constexpr Ret operator()(ArgTypes... args) const {
			return CallFunction<Ret>(Ptr(FunctionAddress), args...);
		}
		
		/**
		 * Call operator, for varadic functions.
		 * This overload only participates 
		 *
		 * \tparam AnyVarArgs varadic arguments.
		 */
		template<class ...AnyVarArgs> requires(IsVaradic)
		constexpr Ret operator()(ArgTypes... args, AnyVarArgs... varargs) const {
			return CallFunction<Ret>(Ptr(FunctionAddress), args..., varargs...);
		}
		
	};
	
	// usings to set IsVaradic eaiser
	
	template<class Ret, uintptr_t FunctionAddress, class ...ArgTypes>
	using VaradicFunctionWrappa = FunctionWrappa<Ret, FunctionAddress, true, ArgTypes...>;
	
	template<class Ret, uintptr_t FunctionAddress, class ...ArgTypes>
	using NonVaradicFunctionWrappa = FunctionWrappa<Ret, FunctionAddress, false, ArgTypes...>;
		
} // namespace util

#endif // CODEUTILS_H