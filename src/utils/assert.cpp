/**
 * SSX-Elfldr
 *
 * (C) 2021-2022 Lily/modeco80 <lily.modeco80@protonmail.ch>
 * under the terms of the MIT license.
 */

//
// Helper code in Utils for handling Runtime's MLSTD_ASSERT()
// and MLSTD_VERIFY()
//

#include <mlstd/Assert.h>
#include <utils/Utils.h>

#ifndef NDEBUG
__attribute__((weak)) void mlstdAssertionFailure(const char* exp, const char* function, const char* file, unsigned line) {
	elfldr::util::DebugOut("MLSTD_ASSERT(%s) failed. File: %s:%d Function %s", exp, file, line, function);
	while(true)
		;
}
#endif

__attribute__((weak)) void mlstdVerifyFailure(const char* exp, const char* file, unsigned line) {
	elfldr::util::DebugOut("MLSTD_VERIFY(%s) failed. File: %s:%d", exp, file, line);
	while(true)
		;
}
