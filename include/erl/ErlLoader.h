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
// is written to just be enough to work for my usage and not much more.
//

namespace elfldr::erl {

	/**
	 * Symbol data type.
	 * Must be pointer-sized.
	 */
	using Symbol = std::uintptr_t;

	/* maybe do this instead?
	struct Symbol {
		
		constexpr bool IsValid() const {
			return _ptr != 0x0;
		}
		
		// usage:
		// auto sym = image->ResolveSymbol("my_function");
		// auto fptr = sym.As<void(*)()>();
		template<class T>
		constexpr T* As() {
			
		}
		
		std::uintptr_t _ptr;
	};
	*/

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

} // namespace elfldr::erl

#endif // ERLLOADER_H