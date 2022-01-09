// General utilities, provided by the "libutils" library.
#ifndef UTILS_H
#define UTILS_H

#include <cstdint>

namespace elfldr::util {

	/**
	 * this should never have been hardcoded
	 * and I'm pissed at myself for letting it slide for as long as I did
	 */
	constexpr static std::size_t MaxPath = 260;

	// it's std::move for elfldr!!!
	template <class T>
	constexpr T&& Move(T&& t) {
		return static_cast<T&&>(t);
	}

	template <class T>
	constexpr T&& Forward(T&& t) {
		return static_cast<T&&>(t);
	}

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

#ifndef ERL
	/**
	 * Global tab level.
	 */
	extern std::uint8_t gTabLevel;
#endif

	/**
	 * Write a message with a prefix.
	 */
	void DebugOut(const char* format, ...);

	// Internal runtime stuff - do not call on your own accord

#ifdef DEBUG
	void _AssertFailed(const char* exp, const char* file, std::uint32_t line);
#endif

	void _VerifyFailed(const char* exp, const char* file, std::uint32_t line);

} // namespace elfldr::util

#ifdef DEBUG
	#define ELFLDR_ASSERT(x)                                         \
		do {                                                         \
			if(!x) {                                                 \
				elfldr::util::_AssertFailed(#x, __FILE__, __LINE__); \
				__builtin_unreachable();                             \
			}                                                        \
		} while(0)
#else
	#define ELFLDR_ASSERT(x)
#endif

#define ELFLDR_VERIFY(x)                                         \
	do {                                                         \
		if(!x) {                                                 \
			elfldr::util::_VerifyFailed(#x, __FILE__, __LINE__); \
			__builtin_unreachable();                             \
		}                                                        \
	} while(0)

#endif // UTILS_H