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

#include <utils/utils.h>

namespace elfldr::detail {

#ifdef DEBUG
	void __AssertFailure(const char* exp, const char* file, unsigned line) {
		util::DebugOut("ELFLDR_ASSERT(%s) failed. File: %s:%d", exp, file, line);
		while(true)
			;
	}
#endif

	void __VerifyFailure(const char* exp, const char* file, unsigned line) {
		util::DebugOut("ELFLDR_VERIFY(%s) failed. File: %s:%d", exp, file, line);
		while(true)
			;
	}

} // namespace elfldr::util