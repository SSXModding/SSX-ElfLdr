// This file declares my own implementation
// of C++ type traits (since we're not using a real C++
// standard library, and libc is all we can use)
#ifndef UTILS_TYPETRAITS_H
#define UTILS_TYPETRAITS_H

namespace elfldr::util {
	
	// declares ::type using,
	// useful since you can just baseclass TypeConstant<T>
	template<class T>
	struct TypeConstant {
		using type = T;
	};
		
	/**
	 * An compile time constant.
	 * Type is deduced and available via the ::type using.
	 *
	 * \tparam constant Constant
	 */
	template<auto constant> 
	struct CompileTimeConstant : public TypeConstant<decltype(constant)> {		
		static constexpr type value = constant;
		
		constexpr operator type() { return value; }
		constexpr type operator()() { return value; }
	};
	
	// This is legacy, use TrueType or FalseType
	template<bool B>
	using BoolConstant = CompileTimeConstant<B>;
	
	using TrueType = CompileTimeConstant<true>;
	using FalseType = CompileTimeConstant<false>;
	

	// This is only here for legacy usage/just to implement it.
	// Any code should use C++20 concepts/requires() where possible
	// for SFINAE/overload rejection.
	
	template<class T, bool B>
	struct EnableIf {};
	
	template<class T>
	struct EnableIf<T, true> : public TypeConstant<T> {};
	
	template<class T, bool B>
	using EnableIfT = EnableIf<T, B>::type;
	
	// This is really only useful in the case
	// where you can't actually put code; e.g. array size or something
	
	template<bool B, class T, class T2>
	struct Conditional : public TypeConstant<T> {};
	
	template<class T, class T2>
	struct Conditional<false, T, T2> : public TypeConstant<T2> {};
	
	template<bool B, class T, class T2>
	using ConditionalT = Conditional<B, T, T2>::type;
	
	
	template<class T, class U>
	struct IsSame : public FalseType {};
	
	template<class T>
	struct IsSame<T, T> : public TrueType {};
	
	template<class T, class U>
	static constexpr auto IsSameV = IsSame<T, U>::value;
	
} // namespace elfldr::util

#endif // UTILS_TYPETRAITS_H