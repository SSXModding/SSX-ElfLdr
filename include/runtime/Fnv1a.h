/**
 * SSX-Elfldr
 *
 * (C) 2021-2022 Lily/modeco80 <lily.modeco80@protonmail.ch>
 * under the terms of the MIT license.
 */

// This is an internal header and provides apis only internal code should touch
// Use the publicly available util::Hash<T> for typed hashing, please :)
// This note does not apply if you're specializing it though

#ifndef UTILS_FNV_H
#define UTILS_FNV_H

#include <stdint.h>
#include <stddef.h>

namespace elfldr::detail {

	/**
	 * FNV1a hash. Pretty self explanatory.
	 *
	 * \returns u32 hash value.
	 * \param[in] input The input data to hash
	 * \param[in] length Size of the input data.
	 * \param[in] lastval Last hashed value, or 0 for a fresh hash.
	 */
	uint32_t fnv1a_hash(const void* input, size_t length, uint32_t lastval);

} // namespace elfldr::util

#endif // UTILS_FNV_H