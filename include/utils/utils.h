/**
 * SSX-Elfldr
 *
 * (C) 2021-2022 Lily/modeco80 <lily.modeco80@protonmail.ch>
 * under the terms of the MIT license.
 */

#ifndef UTILS_H
#define UTILS_H

#include <stddef.h>
#include <stdint.h>

namespace elfldr::util {

	/**
	 * this should never have been hardcoded
	 * and I'm pissed at myself for letting it slide for as long as I did
	 */
	constexpr static size_t MaxPath = 260;

	/**
	 * Write a message with a prefix.
	 */
	void DebugOut(const char* format, ...);

} // namespace elfldr::util

#endif // UTILS_H