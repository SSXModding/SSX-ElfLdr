/**
 * SSX-Elfldr
 *
 * (C) 2021-2022 Lily/modeco80 <lily.modeco80@protonmail.ch>
 * under the terms of the MIT license.
 */

#ifndef MLSTD_HASH_H
#define MLSTD_HASH_H

#include <utils/Utils.h>

#include "mlstd/detail/Fnv1a.h"

namespace mlstd {

	/**
	 * Hash trait template. Provides a function
	 * to hash a given value of type T.
	 */
	template <class T>
	struct Hash {
		// Hash:
		// static uint32_t hash(const T&);
	};

#define HASH_TRIVIAL_SPECIALIZATION(T)                                                  \
	template <>                                                                         \
	struct Hash<T> {                                                                    \
		inline static uint32_t hash(const T& t) {                                       \
			return detail::fnv1a_hash(reinterpret_cast<const void*>(&t), sizeof(T), 0); \
		}                                                                               \
	};

	// trivial specializations for ... trivial types!
	HASH_TRIVIAL_SPECIALIZATION(uint8_t)
	HASH_TRIVIAL_SPECIALIZATION(uint16_t)
	HASH_TRIVIAL_SPECIALIZATION(uint32_t)
	HASH_TRIVIAL_SPECIALIZATION(uint64_t)
	HASH_TRIVIAL_SPECIALIZATION(uintptr_t)
	HASH_TRIVIAL_SPECIALIZATION(int8_t)
	HASH_TRIVIAL_SPECIALIZATION(int16_t)
	HASH_TRIVIAL_SPECIALIZATION(int32_t)
	HASH_TRIVIAL_SPECIALIZATION(int64_t)
	HASH_TRIVIAL_SPECIALIZATION(intptr_t)

#undef HASH_TRIVIAL_SPECIALIZATION

} // namespace mlstd

#endif // MLSTD_HASH_H