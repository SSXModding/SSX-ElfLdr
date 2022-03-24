/**
 * SSX-Elfldr
 *
 * (C) 2021-2022 Lily/modeco80 <lily.modeco80@protonmail.ch>
 * under the terms of the MIT license.
 */

#ifndef ELFLDR_UTILITY_H
#define ELFLDR_UTILITY_H

#include <runtime/TypeTraits.h>
#include <string.h> // memcpy()
#include <stddef.h>

namespace elfldr {

	template <class T>
	constexpr T&& Move(T&& t) {
		return static_cast<T&&>(t);
	}

	template <class T>
	constexpr T&& Forward(T&& t) {
		return static_cast<T&&>(t);
	}

	/**
	 * Pair. Simple enough.
	 * Data layout is POD, and C-compatible if used with C-compatible types.
	 */
	template <class T1, class T2>
	struct Pair {
		constexpr Pair() = default;

		/**
		 * Initialize the pair with values.
		 */
		constexpr Pair(T1&& t1, T2&& t2)
			: first(Move(t1)),
			  second(Move(t2)) {
		}

		T1 first;
		T2 second;
	};

	// deduction guide so Pair{1, 0.24} is well-formed.
	template <class T1, class T2>
	Pair(T1, T2) -> Pair<T1, T2>;

	/**
	 * Typed transfer utilities.
	 *
	 * Automatically switches to more/less performant code
	 * per each class.
	 */
	template <class T>
	struct TypedTransfer {
		inline static void Copy(T* dest, const T* source, size_t length) requires(IsTrivallyCopyableV<T>) {
			// For performance reasons, if we can copy via memcpy(),
			// prefer that.
			memcpy(dest, source, length * sizeof(T));
		}

		inline static void Copy(T* dest, const T* source, size_t length) {
			// If we can't, oh well, that's OK too.
			for(size_t i = 0; i < length; ++i)
				new(dest[i]) T(source);
		}

		// Move() later on
	};

	/**
	 * The worst casting function ever.
	 * It works, though.
	 *
	 * The name of this function sums it up well.
	 * This is UB, and I'm only doing this because it works.
	 * It lets my code funging with pointers
	 * be constexpr and boil away very quickly, so I'm happy.
	 *
	 * \tparam Dest Destination cast type.
	 * \tparam Source Source type. Guessed, so semantically, "..._cast<T>" can be replaced easily
	 *
	 * \param[in] source Source.
	 * \return Dest-casted value.
	 */
	template <class Dest, class Source>
	constexpr Dest UBCast(Source source) {
		// FIXME: actually using this at compile time
		// causes some uninitialized member errors.

		union {
			Source src;
			Dest dst;
		} u { .src = source };

		// Does this actually cause any problems? It'd just truncate data,
		// I think?
		// Either way, the safer, the better.
		// Especially when I'm doing union type punning.
		//
		//(something C++ itself considers UB,
		// although GCC still lets it work even with strict aliasing.
		//
		// People point at me C99/C11 techinically "allowing" union punning, but
		// in reality it's GCC behaviour which truly allows it
		// (even compiling C++ where strict aliasing).
		//)
		static_assert(sizeof(u) == sizeof(Source), "anonymous union used for punning must be sizeof(Source). Dest may not be bigger.");

		return u.dst;
	}

} // namespace elfldr

#endif // ELFLDR_UTILITY_H
