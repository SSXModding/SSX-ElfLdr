/**
 * SSX-Elfldr
 *
 * (C) 2021-2022 Lily/modeco80 <lily.modeco80@protonmail.ch>
 * under the terms of the MIT license.
 */

// This header provides ELFLDR_ASSERT() and ELFLDR_VERIFY(),
// alongside definitions of the internal functions
// which the two macros dispatch to.
//
// ELFLDR_ASSERT() behaves similarly to assert(),
// in that when in release, the statement boils away,
// and the runtime code for it also is gone.
//
// ELFLDR_VERIFY(), however, does not go away,
// and is always active regardless of mode.
//
// Rule is to put ELFLDR_ASSERT() where something should
// ideally never happen, and put ELFLDR_VERIFY() where it
// absolutely, totally, should not, ever in the history of the
// universe happen, and if it does, it would wreak havoc on all
// of Earth.
//
// I'm only kidding of course, but, for the most part,
// ELFLDR_VERIFY() is used for sanity checking I'd always like in
// the release build.

#ifndef ELFLDR_ASSERT_H
#define ELFLDR_ASSERT_H

#ifdef __cplusplus
extern "C" {
#endif

#ifndef NDEBUG
void __Elfldr__AssertFailure(const char* exp, const char* function, const char* file, unsigned line);
#endif
void __Elfldr__VerifyFailure(const char* exp, const char* file, unsigned line);

#ifdef __cplusplus
}
#endif

// NB: We don't implement the craziness that the C standard assert() allows,
// just enough to assert/verify. Don't go putting this into expressions.

#ifndef NDEBUG
	#define ELFLDR_ASSERT(x)                                                          \
		do {                                                                          \
			if(!(x)) {                                                                \
				__Elfldr__AssertFailure(#x, __PRETTY_FUNCTION__, __FILE__, __LINE__); \
				__builtin_unreachable();                                              \
			}                                                                         \
		} while(0)
#else
	#define ELFLDR_ASSERT(x)
#endif

#define ELFLDR_VERIFY(x)                                     \
	do {                                                     \
		if(!(x)) {                                           \
			__Elfldr__VerifyFailure(#x, __FILE__, __LINE__); \
			__builtin_unreachable();                         \
		}                                                    \
	} while(0)


#define ELFLDR_UNREACHABLE() __builtin_unreachable()

#endif // ELFLDR_ASSERT_H
