#ifndef ERLLOADER_H
#define ERLLOADER_H

#include <cstdint>

namespace elfldr {
	
	/**
	 * An ERL image.
	 */
	struct ErlImage {
		
		/**
		 * Resolve a symbol inside of this ERL image.
		 */
		void* ResolveSymbol(const char* symbolName) const;
		
		
	};
	
	/**
	 * malloc() callback type for the ERL loader.
	 */
	using ErlMalloc = void*(*)(std::uint32_t);
	
	/**
	 * free() callback type for the ERL loader.
	 */
	using ErlFree = void(*)(void*);
	
	/**
	 * The warning callback is called upon loader warnings.
	 */
	using ErlWarningCallback = void(*)(const char*);
	
	
	// TODO for standalone: remove this and just set it to malloc/free?
	// the flexibility might actually be nice, I'll have to see.
	
	/**
	 * Set the ERL loader's memory allocation/deallocation
	 * functions.
	 */
	void SetErlAllocationFunctions(ErlMalloc malloc, ErlFree free);
	
	
	/**
	 * Set the warning callback for the ERL loader.
	 */
	void SetErlWarningCallback(ErlWarningCallback warning);
	
	/**
	 * Load and relocate a .erl file.
	 *
	 * \param[in] path ERL path.
	 * \returns ErlImage handle, or nullptr on error.
	 */
	ErlImage* LoadErl(const char* path);
	
}

#endif // ERLLOADER_H