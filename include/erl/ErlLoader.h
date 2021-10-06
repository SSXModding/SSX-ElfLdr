#ifndef ERLLOADER_H
#define ERLLOADER_H

#include <cstdint>

namespace elfldr::erl {
	
	/**
	 * An ERL image.
	 */
	struct Image {
		
		virtual ~Image() = default;
		
		/**
		 * Resolve a symbol inside of this ERL image.
		 */
		virtual void* ResolveSymbol(const char* symbolName) = 0;
		
		
	};
	
	/**
	 * malloc() callback type for the ERL loader.
	 */
	using Malloc = void*(*)(std::uint32_t);
	
	/**
	 * free() callback type for the ERL loader.
	 */
	using Free = void(*)(void*);
	
	/**
	 * The warning callback is called upon loader warnings.
	 */
	using WarningCallback = void(*)(const char*);
	
	/**
	 * Set the ERL loader's memory allocation/deallocation
	 * functions.
	 */
	void SetAllocationFunctions(Malloc erlmalloc, Free erlfree);
	
	
	/**
	 * Set the warning callback for the ERL loader.
	 */
	void SetWarningCallback(WarningCallback warning);
	
	/**
	 * Load and relocate a .erl file.
	 *
	 * \param[in] path ERL path.
	 * \returns ErlImage handle (allocated with the ERL allocator), or nullptr on error.
	 */
	Image* LoadErl(const char* path);
	
	/**
	 * Destroy a loaded ERL image.
	 */
	void DestroyErl(Image* theImage);
	
}

#endif // ERLLOADER_H