/**
 * SSX-Elfldr
 *
 * (C) 2021-2022 Lily/modeco80 <lily.modeco80@protonmail.ch>
 * under the terms of the MIT license.
 */

//
// Helper code in Utils for handling Runtime's ELFLDR_ASSERT()
// and ELFLDR_VERIFY()
//

#include <mlstd/Assert.h>
#include <utils/Utils.h>

#ifndef NDEBUG
__attribute__((weak)) void __Elfldr__AssertFailure(const char* exp, const char* function, const char* file, unsigned line) {
	elfldr::util::DebugOut("ELFLDR_ASSERT(%s) failed. File: %s:%d Function %s", exp, file, line, function);
	while(true)
		;
}
#endif

__attribute__((weak)) void __Elfldr__VerifyFailure(const char* exp, const char* file, unsigned line) {
	elfldr::util::DebugOut("ELFLDR_VERIFY(%s) failed. File: %s:%d", exp, file, line);
	while(true)
		;
}
