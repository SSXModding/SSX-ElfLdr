#include <erl/ErlLoader.h>

#include "elf.h"

#include "utils/Allocator.h"
#include "HashTable.h"

#include <utils/Expected.h>
#include <utils/FioFile.h>


// if building debug, then we can afford to be a bit more chatty.
#ifdef DEBUG
	#include <utils.h> // libutils

	#define ERL_DEBUG_PRINTF(format, ...) elfldr::util::DebugOut("[Erl] " format, ##__VA_ARGS__)
#else
	#define ERL_DEBUG_PRINTF(format, ...)
#endif

namespace elfldr::erl {

	// all possible ERL load errors
	enum class ErlLoadError {
		FileNotFound,   /** File does not exist on disk */
		NotElf,         /** Not a ELF file */
		NotMips,        /** ELF machine type is not MIPS R5900 */
		SizeMismatch,   /** Some data structure size didn't match up our structures */
		OomHit,         /** Could not allocate additional resources */
		NotRelocatable, /** ELF is not relocatable */
		NoSymbols       /** No symbols */
	};

	template<class T>
	using ErlResult = util::Expected<T, ErlLoadError>;


	/**
	 * This class is actually what we allocate
	 * when we give an Image* to people.
	 * It includes all the private members we might need.
	 *
	 * Also provides the implementation of Image::ResolveSymbol.
	 */
	struct ImageImpl : public Image {
		~ImageImpl() {
			ERL_DEBUG_PRINTF("~ImageImpl()");
		}

		ErlResult<void> Load(const char* path) {
			// Stage 1: open the file.
			// If this doesn't work, then the user specified an invalid path, so we should fail with that
			util::FioFile file;

			file.Open(path, FIO_O_RDONLY);

			if(!file)
				return ErlLoadError::FileNotFound;

			// Stage 2: Read the ELF header.
			elf_header_t header{};
			file.Read(util::UBCast<void*>(&header), sizeof(header));

			// Check ELF signature to make sure this is actually an ELF file.
			if(header.e_ident.cook.ei_magic[0] != '\77' && header.e_ident.cook.ei_magic[1] != 'E' && header.e_ident.cook.ei_magic[2] != 'L' && header.e_ident.cook.ei_magic[3] != 'F')
				return ErlLoadError::NotElf;

			// Check if this is a relocatable ELF.
			if(header.e_type != REL_TYPE)
				return ErlLoadError::NotRelocatable;

			// FIXME: Guard for r5900 mips ELF's.
			//  ORIGINAL ERL LIBRARY DOES NOT DO THIS!!!!!

			if(sizeof(elf_section_t) != header.e_shentsize)
				return ErlLoadError::SizeMismatch;

			// Let's read the section table:
			auto* sections = new elf_section_t[header.e_shnum];
			if(!sections)
				return ErlLoadError::OomHit;

			file.Seek(header.e_shoff, FIO_SEEK_SET);
			file.Read(sections, header.e_shnum * sizeof(elf_section_t));

			// Let's read .shstrtab:
			String strtab;
			strtab.Resize(sections[header.e_shstrndx].sh_size);

			elfldr::util::DebugOut("penis %d %d %x", sections[header.e_shstrndx].sh_offset, strtab.length(), strtab.data());

			file.Seek(sections[header.e_shstrndx].sh_offset, FIO_SEEK_SET);
			file.Read(strtab.data(), strtab.length());

			for(int i = 1; i < header.e_shnum; ++i) {
				auto& section = sections[i];

				elfldr::util::DebugOut("[Erl] Section %d name: %s (name offset %d)", i, strtab.data(), section.sh_name);
			}

			// No error occurred!
			return util::NO_ERROR<ErlLoadError>;
		}

		Symbol ResolveSymbol(const char* symbolName) override {
			return 7000;
		}

		HashTable<String, Symbol> symbol_table;
	};

	// TODO: put this in utils? it might be ok to have round

	template <class ScopeExitT>
	struct ScopeExitGuard {
		constexpr ScopeExitGuard(ScopeExitT se)
			: scope_exited(util::Move(se)) {
		}

		// call the exit functor
		constexpr ~ScopeExitGuard() {
			if(should_call)
				scope_exited();
		}

		void DontCall() {
			should_call = false;
		}

	   private:
		ScopeExitT scope_exited;
		bool should_call{true};
	};

	Image* LoadErl(const char* path) {
		auto* image = new ImageImpl();

		ScopeExitGuard guard([&]() {
			delete image;
		});

		ERL_DEBUG_PRINTF("Attempting to load ERL \"%s\"", path);

		auto res = image->Load(path);
		if(res.HasError()) {
			// We know there's an error
			util::DebugOut("[Erl] Error %d loading ERL \"%s\"", res.Error(), path);
			return nullptr;
		}

		// OK!
		// We've loaded everything, and it's seemed to worked!
		// Woo-hoo!
		guard.DontCall();
		return image;
	}

	void DestroyErl(Image* theImage) {
		delete theImage;
	}

} // namespace elfldr::erl