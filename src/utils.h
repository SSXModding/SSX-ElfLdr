// General utilities.

#ifndef UTILS_H
#define UTILS_H

#include <cstdint>

namespace elfldr::util {
	
	/**
	 * Write a message with a prefix.
	 */
	void DebugOut(const char* format, ...);
	
}

#endif // UTILS_H