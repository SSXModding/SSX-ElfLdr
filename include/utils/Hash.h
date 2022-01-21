/**
 * SSX-Elfldr
 *
 * (C) 2021-2022 Lily/modeco80 <lily.modeco80@protonmail.ch>
 * under the terms of the MIT license.
 */

#ifndef UTILS_HASH_H
#define UTILS_HASH_H


#include <utils/Fnv1a.h>
#include <utils/utils.h>

namespace elfldr::util {

	/**
	 * Hash template. Provides a function
	 * to hash a given value of type T.
	 */
	template <class T>
	struct Hash {
		// Hash:
		//static std::uint32_t hash(const T&);
	};


	// TODO: more specializations, here.

} // namespace elfldr::util

#endif // UTILS_HASH_H