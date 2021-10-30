//
// Helper code in libutils for handling ELFLDR_ASSERT()
// and ELFLDR_VERIFY()
//

#include <utils.h>

namespace elfldr::util {

#ifdef DEBUG
	void _AssertFailed(const char* exp, const char* file, std::uint32_t line) {
		DebugOut("ELFLDR_ASSERT() expression failed!!!!\nExpression: %s\nFile: %s\nLine: %d", exp, file, line);
		while(true)
			;
	}
#endif

	void _VerifyFailed(const char* exp, const char* file, std::uint32_t line) {
		DebugOut("ELFLDR_VERIFY() expression failed!!!!\nExpression: %s\nFile: %s\nLine: %d", exp, file, line);
		while(true)
			;
	}

} // namespace elfldr::util