/**
 * SSX-Elfldr
 *
 * (C) 2021-2022 Lily/modeco80 <lily.modeco80@protonmail.ch>
 * under the terms of the MIT license.
 */

// This header provides MLSTD_ASSERT() and MLSTD_VERIFY(),
// alongside definitions of the internal functions
// which the two macros dispatch to.
//
// MLSTD_ASSERT() behaves similarly to assert(),
// in that when in release, the statement boils away,
// and the runtime code for it also is gone.
//
// MLSTD_VERIFY(), however, does not go away,
// and is always active regardless of mode.
//
// Rule is to put MLSTD_ASSERT() where something should
// ideally never happen, and put MLSTD_VERIFY() where it
// absolutely, totally, should not, ever in the history of the
// universe happen, and if it does, it would wreak havoc on all
// of Earth.
//
// I'm only kidding of course, but, for the most part,
// MLSTD_VERIFY() is used for sanity checking I'd always like in
// the release build.

#ifndef MLSTD_ASSERT_H
#define MLSTD_ASSERT_H

#ifdef __cplusplus
extern "C" {
#endif

#ifndef NDEBUG
extern "C" void mlstdAssertionFailure(const char* exp, const char* function, const char* file, unsigned line);
#endif
extern "C" void mlstdVerifyFailure(const char* exp, const char* file, unsigned line);

#ifdef __cplusplus
}
#endif

// NB: We don't implement the craziness that the C standard assert() allows,
// just enough to assert/verify. Don't go putting this into expressions.

#ifndef NDEBUG
	#define MLSTD_ASSERT(x)                                                         \
		do {                                                                        \
			if(!(x)) {                                                              \
				mlstdAssertionFailure(#x, __PRETTY_FUNCTION__, __FILE__, __LINE__); \
				__builtin_unreachable();                                            \
			}                                                                       \
		} while(0)
#else
	#define MLSTD_ASSERT(x)
#endif

#define MLSTD_VERIFY(x)                                 \
	do {                                                \
		if(!(x)) {                                      \
			mlstdVerifyFailure(#x, __FILE__, __LINE__); \
			__builtin_unreachable();                    \
		}                                               \
	} while(0)

#define MLSTD_UNREACHABLE() __builtin_unreachable()

#endif // MLSTD_ASSERT_H
