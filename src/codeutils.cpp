// implementation tu for a couple things I found kinda bloaty
// to be inlines

#include "utils.h"
#include "codeutils.h"

namespace elfldr::util {
		
	void ReplaceString(void* addr, const char* string) {
		DebugOut("Replacing string \"%s\" at %p with \"%s\"...", reinterpret_cast<char*>(addr), addr, string);
		memcpy(addr, string, strlen(string) + 1);
	}

	void WriteString(void* addr, const char* string) {
		DebugOut("Writing new string at %p with \"%s\"...", addr, string);
		memcpy(addr, string, strlen(string) + 1);
	}

}