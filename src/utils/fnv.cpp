// FNV1a hash

#include <utils.h>

namespace elfldr::util {
	
	 std::uint32_t fnv1a_hash(const void* input, std::size_t length, std::uint32_t lastval) {
		 auto* ptr = UBCast<std::uint8_t*>(input);
		 auto* end = ptr + length;
		 
		 while(ptr < end) {
			 lastval ^= *reinterpret_cast<std::uint32_t*>(ptr++);
			 
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