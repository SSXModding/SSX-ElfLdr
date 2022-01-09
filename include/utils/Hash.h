#ifndef UTILS_HASH_H
#define UTILS_HASH_H

// std::hash like primitive

#include <utils/Fnv1a.h>
#include <utils/utils.h>

namespace elfldr::util {

	template <class T>
	struct Hash {
		// Hash:
		//static std::uint32_t hash(const T&);
	};


	// TODO: more specializations.

} // namespace elfldr::util

#endif // UTILS_HASH_H