#ifndef UTILS_HASH_H
#define UTILS_HASH_H

// std::hash like primitive

#include <utils/fnv.h>
#include <utils/utils.h>

#include <utils/String.h>

namespace elfldr::util {

	template <class T>
	struct Hash {
		// Hash:
		//static std::uint32_t hash(const T&);
	};

	template <>
	struct Hash<String> {
		inline static std::uint32_t hash(const String& str) {
			return util::fnv1a_hash(UBCast<void*>(str.data()), str.length(), 0);
		}
	};

	// TODO: more specializations.

} // namespace elfldr::util

#endif // UTILS_HASH_H