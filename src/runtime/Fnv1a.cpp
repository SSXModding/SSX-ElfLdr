/**
 * SSX-Elfldr
 *
 * (C) 2021-2022 Lily/modeco80 <lily.modeco80@protonmail.ch>
 * under the terms of the MIT license.
 */

// FNV1a hash algorithm.

#include <runtime/Hash.h>


//#define USE_PRIME // Use magic prime from original C FNV1a implementation

namespace elfldr::detail {
		uint32_t fnv1a_hash(const void* input, size_t length, uint32_t lastval) {
			auto* ptr = reinterpret_cast<const uint8_t*>(input);
			auto* end = ptr + length;

			while(ptr < end) {
				lastval ^= *reinterpret_cast<const uint32_t*>(ptr++);

#ifdef USE_PRIME
				// this magic prime is referenced in the original
				// C implementation of FNV1a

				lastval *= 0x01000193;
#else
				lastval += (lastval << 1) + (lastval << 4) + (lastval << 7) + (lastval << 8) + (lastval << 24);
#endif
			}

			return lastval;
		}

} // namespace elfldr::util