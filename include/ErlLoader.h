#ifndef ERLLOADER_H
#define ERLLOADER_H

#include <cstdint>

namespace elfldr {
	
	struct ErlImage {
		
		/**
		 * Resolve a symbol inside of this ERL image.
		 */
		void* ResolveSymbol(const char* symbolName) const;
		
		
	};
	
	/**
	 * Load and relocate a .erl file.
	 *
	 * \param[in] path ERL path.
	 * \returns ErlImage handle, or nullptr on error.
	 */
	ErlImage* LoadErl(const char* path);
	
}

#endif // ERLLOADER_H