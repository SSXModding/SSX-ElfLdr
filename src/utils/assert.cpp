/**
 * SSX-Elfldr
 *
 * (C) 2021-2022 Lily/modeco80 <lily.modeco80@protonmail.ch>
 * under the terms of the MIT license.
 */

//
// Helper code in libutils for handling ELFLDR_ASSERT()
// and ELFLDR_VERIFY()
//

#include <utils/utils.h>

namespace elfldr::util {

#ifdef DEBUG
	void _AssertFailed(const char* exp, const char* file, uint32_t line) {
		DebugOut("ELFLDR_ASSERT() expression failed!!!!\nExpression: %s\nFile: %s\nLine: %d", exp, file, line);
		while(true)
			;
	}
#endif

	void _VerifyFailed(const char* exp, const char* file, uint32_t line) {
		DebugOut("ELFLDR_VERIFY() expression failed!!!!\nExpression: %s\nFile: %s\nLine: %d", exp, file, line);
		while(true)
			;
	}

} // namespace elfldr::util