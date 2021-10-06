// Helper code in libutils for handling ELFLDR_ASSERT()
// and ELFLDR_ASSERTREL (ELFLDR_ASSERT which is kept in for release.)

#include <utils.h> // DebugOut or bx::printf for ERL
//#include <GameApi.h>

namespace elfldr::util {
	
#ifdef DEBUG
	void DebugAssertFailed(const char* exp, const char* file, std::uint32_t line) {
		DebugOut("Debug assertion failed!!!!\nExpression: %s\nFile: %s\nLine: %d", file, line);

		// Spin forever.
		while(true);
	}
#endif

	void RelAssertFailed(const char* exp, const char* file, std::uint32_t line) {
		DebugOut("Release assertion failed!!!!\nExpression: %s\nFile: %s\nLine: %d", file, line);

		// Spin forever.
		while(true);
	}
	
} // namespace elfldr::util