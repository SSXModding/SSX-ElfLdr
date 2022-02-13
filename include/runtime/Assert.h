/**
 * SSX-Elfldr
 *
 * (C) 2021-2022 Lily/modeco80 <lily.modeco80@protonmail.ch>
 * under the terms of the MIT license.
 */

#ifndef ELFLDR_ASSERT_H
#define ELFLDR_ASSERT_H

// internal detail namespace,
// implement these in your code
namespace elfldr::detail {

#ifdef DEBUG
	void __AssertFailure(const char* exp, const char* file, unsigned line);
#endif

	void __VerifyFailure(const char* exp, const char* file, unsigned line);

} // namespace elfldr::detail

// TODO: source location object? would be cool
// Another TODO: maybe make this C compatiable?
// 			     if required i guess

#ifdef DEBUG
	#define ELFLDR_ASSERT(x)                                             \
		do {                                                             \
			if(!(x)) {                                                   \
				elfldr::detail::__AssertFailure(#x, __FILE__, __LINE__); \
				__builtin_unreachable();                                 \
			}                                                            \
		} while(0)
#else
	#define ELFLDR_ASSERT(x)
#endif

#define ELFLDR_VERIFY(x)                                             \
	do {                                                             \
		if(!(x)) {                                                   \
			elfldr::detail::__VerifyFailure(#x, __FILE__, __LINE__); \
			__builtin_unreachable();                                 \
		}                                                            \
	} while(0)

#endif // ELFLDR_ASSERT_H
