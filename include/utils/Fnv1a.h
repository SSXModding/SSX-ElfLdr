#ifndef UTILS_FNV_H
#define UTILS_FNV_H

namespace elfldr::util {

	/**
	 * FNV1a hash. Pretty self explanatory.
	 *
	 * \returns u32 hash value.
	 * \param[in] input The input data to hash
	 * \param[in] length Size of the input data.
	 * \param[in] lastval Last hashed value, or 0 for a fresh hash.
	 */
	std::uint32_t fnv1a_hash(const void* input, std::size_t length, std::uint32_t lastval);

} // namespace elfldr::util

#endif // UTILS_FNV_H