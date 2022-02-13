/**
 * SSX-Elfldr Runtime
 *
 * (C) 2021-2022 Lily/modeco80 <lily.modeco80@protonmail.ch>
 * under the terms of the MIT license.
 */

// This file declares my own implementation
// of C++ type traits in Runtime.

#ifndef UTILS_TYPETRAITS_H
#define UTILS_TYPETRAITS_H

namespace elfldr {

	// declares ::type using,
	// useful since you can just baseclass TypeConstant<T>
	template <class T>
	struct TypeConstant {
		using type = T;
	};

	/**
	 * An compile time constant.
	 * Type is deduced and available via the ::type using.
	 *
	 * \tparam constant Constant
	 */
	template <auto constant>
	struct CompileTimeConstant {
		using type = decltype(constant);

		static constexpr type value = constant;

		constexpr operator type() {
			return value;
		}

		constexpr auto operator()() -> type {
			return value;
		}
	};

	// This is legacy, use TrueType or FalseType
	template <bool B>
	using BoolConstant = CompileTimeConstant<B>;

	using TrueType = CompileTimeConstant<true>;
	using FalseType = CompileTimeConstant<false>;

	// This is only here for legacy usage/just to implement it.
	// Any code should use C++20 concepts/requires() where possible
	// for SFINAE/overload rejection.

	template <class T, bool B>
	struct EnableIf {};

	template <class T>
	struct EnableIf<T, true> : public TypeConstant<T> {};

	template <class T, bool B>
	using EnableIfT = typename EnableIf<T, B>::type;

	// This is really only useful in the case
	// where you can't actually put code; e.g. array size or something

	template <bool B, class T, class T2>
	struct Conditional : public TypeConstant<T> {};

	template <class T, class T2>
	struct Conditional<false, T, T2> : public TypeConstant<T2> {};

	template <bool B, class T, class T2>
	using ConditionalT = typename Conditional<B, T, T2>::type;

	template <class T, class U>
	struct IsSame : public FalseType {};

	template <class T>
	struct IsSame<T, T> : public TrueType {};

	template <class T>
	struct IsTriviallyCopyable : public BoolConstant<__is_trivially_copyable(T)> {};

	template <class T>
	[[maybe_unused]] inline constexpr auto IsTrivallyCopyableV = IsTriviallyCopyable<T>::value;

	template <class T, class U>
	[[maybe_unused]] inline constexpr auto IsSameV = IsSame<T, U>::value;

} // namespace elfldr

#endif // UTILS_TYPETRAITS_H