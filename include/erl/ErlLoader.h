#ifndef ERLLOADER_H
#define ERLLOADER_H

#include <cstdint>

// The API for Liberl.
//
// Liberl is a C++ replacement library for the PS2SDK ERL
// loader library, designed to be lean (which unfortunately means
// a minor sacrifice of features.).
//
// Liberl has no support for imports (from either the Elfldr executable, or another ERL),
// no support for dependencies (which is intentional, seeing how that would be a pain),
// no support for "global" symbols, and generally,
// is written to just be enough to work.
//

namespace elfldr::erl {
	
	// symbol data type
	using Symbol = std::uintptr_t;
	
	/**
	 * An ERL image.
	 */
	struct Image {
		
		virtual ~Image() = default;
		
		/**
		 * Resolve a ERL-local symbol.
		 *
		 * \returns Symbol address if found, or 0x0 (nullptr).
		 *
		 * \param[in] symbolName The name of the symbol to resolve.
		 */
		virtual Symbol ResolveSymbol(const char* symbolName) = 0;
		
		
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
	 * Set the ERL loader's memory allocation/deallocation
	 * functions.
	 */
	void SetAllocationFunctions(Malloc erlmalloc, Free erlfree);
	
	/**
	 * Load and relocate a .erl file.
	 *
	 * \param[in] path ERL path.
	 * \returns Image handle (allocated with the ERL allocator), or nullptr on error.
	 */
	Image* LoadErl(const char* path);
	
	/**
	 * Destroy a loaded ERL image.
	 */
	void DestroyErl(Image* theImage);
	
}

#endif // ERLLOADER_H