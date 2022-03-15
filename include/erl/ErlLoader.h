/**
 * SSX-Elfldr/LibERL
 *
 * (C) 2021-2022 Lily/modeco80 <lily.modeco80@protonmail.ch>
 * under the terms of the MIT license.
 */

#ifndef ERLLOADER_H
#define ERLLOADER_H

#include <stdint.h>
#include <runtime/Utility.h>

#include <runtime/Expected.h>
#include <runtime/String.h>


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
			return UBCast<int>(_ptr) != -1;
		}

		uintptr_t AsRaw() const {
			return _ptr;
		}

		template <class T>
		constexpr T* As() const {
			return UBCast<T*>(_ptr);
		}

		// do not touch, this is only to keep the POD contract true
		uintptr_t _ptr;
	};

	// TODO: it would be really nice to allow state

	// all possible ERL load errors
	enum class ErlLoadError {
		FileNotFound, /** File does not exist on disk */
		NotElf,		  /** Not a ELF file */
		//	NotMips,		/** ELF machine type is not MIPS R5900 */
		SizeMismatch,	/** Some data structure size didn't match up our structures */
		NotRelocatable, /** ELF is not relocatable */
		//	NoSymbols,		/** No symbols */
		RelocationError /** Internal error relocating symbol */
	};

	/**
	 * Convert a ErlLoadError to string.
	 */
	static StringView LoadErrorToString(ErlLoadError e) {
		constexpr static const char* table[] {
			"ERL file not found",
			"Not ELF file",
			//	"Not MIPS",
			"Critical structure size mismatch",
			"Not a relocatable ELF",
			//	"No symbols",
			"Internal error relocating symbol :("
		};
		return table[static_cast<size_t>(e)];
	}

	/**
	 * Helper typedef for the load result.
	 */
	template <class T>
	using LoadResult = Expected<T, ErlLoadError>;

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

	// may not be needed.. yet
	/**
	 * Add a global symbol to the ERL loader.
	 * This symbol when spotted in an ERL will be slotted
	 * in with the approiate address.
	 */
	//void AddGlobalSymbol(const char* name, Symbol address);

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