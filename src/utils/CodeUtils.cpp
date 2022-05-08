/**
 * SSX-Elfldr
 *
 * (C) 2021-2022 Lily/modeco80 <lily.modeco80@protonmail.ch>
 * under the terms of the MIT license.
 */

// implementation tu for a couple things I found kinda bloaty
// to be inlines

#include <mlstd/Assert.h>
#include <utils/CodeUtils.h>
#include <utils/Utils.h>

namespace elfldr::util {

	void ReplaceString(void* addr, const char* string) {
		DebugOut("Replacing string \"%s\" at %p: \"%s\"...", reinterpret_cast<char*>(addr), addr, string);
		memcpy(addr, string, strlen(string) + 1);
	}

	void WriteString(void* addr, const char* string) {
		DebugOut("Writing string at %p: \"%s\"...", addr, string);
		memcpy(addr, string, strlen(string) + 1);
	}

} // namespace elfldr::util