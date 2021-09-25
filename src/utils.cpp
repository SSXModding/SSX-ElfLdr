#include "utils.h"

#include <cstdarg>
#include <cstring>
#include <cstdio>

namespace elfldr::util {
	
	
	void DebugOut(const char* format, ...) {
		char buf[256]{};
		va_list val;
		va_start(val, format);
			// pain.
			memcpy(&buf[0], "[ElfLdr] ", sizeof("[ElfLdr]"));
			vsnprintf(&buf[sizeof("[ElfLdr]")], (sizeof(buf) - sizeof("[ElfLdr]") - 1), format, val);
			puts(buf);
		va_end(val);
	}
	
}