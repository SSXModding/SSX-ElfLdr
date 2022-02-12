/**
 * SSX-Elfldr/LibERL
 *
 * (C) 2021-2022 Lily/modeco80 <lily.modeco80@protonmail.ch>
 * under the terms of the MIT license.
 */

#ifndef ERLLOADER_H
#define ERLLOADER_H

#include <utils/utils.h>


// The public API surface for LibErl.
//
// LibErl is a C++ replacement library for the PS2SDK ERL
// loader library, designed to be lean (which unfortunately means
// a minor sacrifice of features.).
//
// LibErl currently has no support for imports (from either the Elfldr executable, or another ERL),
// no support for dependencies (which is intentional to ease work.),
// no support for "global" symbols, and generally,
// is written to just be enough to work for my usage in ElfLdr and not much more.
//

namespace elfldr::erl {

	struct Symbol {
		constexpr Symbol() = default;

		constexpr Symbol(uintptr_t p)
			: _ptr(p) {
		}

		[[nodiscard]] constexpr bool IsValid() const {
			return util::UBCast<int>(_ptr) != -1;
		}

		uintptr_t AsRaw() const {
			return _ptr;
		}

		template <class T>
		constexpr T* As() const {
			return util::UBCast<T*>(_ptr);
		}

		// do not touch, this is only to keep the POD contract true
		uintptr_t _ptr;
	};

	/**
	 * An ERL image.
	 */
	struct Image {
		virtual ~Image() = default;

		/**
		 * Resolve an ERL-local symbol.
		 *
		 * \returns Symbol address if found, or 0x0 (nullptr).
		 *
		 * \param[in] symbolName The name of the symbol to resolve.
		 */
		virtual Symbol ResolveSymbol(const char* symbolName) = 0;

		virtual const char* GetFileName() const = 0;
	};

	/**
	 * Add a global symbol to the ERL loader.
	 * This symbol when spotted in an ERL will be slotted
	 * in with the approiate
	 */
	void AddGlobalSymbol(const char* name, Symbol address);

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