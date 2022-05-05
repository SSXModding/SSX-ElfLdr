/**
 * SSX-Elfldr/LibERL
 *
 * (C) 2021-2022 Lily/modeco80 <lily.modeco80@protonmail.ch>
 * under the terms of the MIT license.
 */

#include <elf.h>
#include <elfldr/ElfLoader.h>
#include <erl/ErlLoader.h>
#include <runtime/Allocator.h>
#include <runtime/DynamicArray.h>
#include <runtime/HashTable.h>
#include <runtime/ScopeExitGuard.h>
#include <runtime/String.h>
#include <utils/codeutils.h>
#include <utils/FioFile.h>

#define DEBUG

#define ERL_RELEASE_PRINTF(format, ...) elfldr::util::DebugOut("[ERL] " format, ##__VA_ARGS__)

// if building debug, then we can afford to be a bit more chatty.
#ifdef DEBUG
	#define ERL_DEBUG_PRINTF(format, ...) elfldr::util::DebugOut("[ERL] " format, ##__VA_ARGS__)
#else
	#define ERL_DEBUG_PRINTF(format, ...)
#endif

namespace elfldr::erl {

	constexpr uint32_t Align(uint32_t alignment_value, int align) {
		align--;
		if(alignment_value & align) {
			alignment_value |= align;
			alignment_value++;
		}
		return alignment_value;
	}

	template <class T, size_t BitCount>
	constexpr T SignExt(const T t) {
		// See https://graphics.stanford.edu/~seander/bithacks.html#FixedSignExtend
		// this is pretty much a C++20-ified version
		struct SignExtender {
			const T sign_extended : BitCount;
		} extender {
			.sign_extended = t
		};
		return extender.sign_extended;
	}

	// TODO: Array<Image*>
	// 	or HashTable<String(View), Image*> gLoadedImages?
	//		for dependency support

	/**
	 * Implementation of ERL image things.
	 */
	struct ImageImpl {
		~ImageImpl() {
			ERL_DEBUG_PRINTF("~ImageImpl()");
		}

		Symbol ResolveSymbol(const char* symbolName) {
			// FIXME: Does this need to construct a temporary string *EVERY* time it gets called?
			//        that's a new[], memcpy(), and delete[] just to look up a symbol.
			//        Too expensive for my liking, but it shouldn't happen frequently.

			if(auto sym = symbol_table.MaybeGet(String(symbolName)); sym != nullptr) {
				return *sym;
			}

			return -1;
		}

		[[nodiscard]] const char* GetFileName() const {
			return filename.c_str();
		}

		// Implementation data

		HashTable<String, Symbol> symbol_table;
		String filename;
		DynamicArray<uint8_t> bytes;
	};

	/**
	 * Loader object.
	 * Encapsulates all state which is required during load time.
	 */
	struct ErlLoader {
		/**
		 * Constructor.
		 * \param[in] path Path to ERL
		 * \param[in,out] impl Reference to impl object to fill. Mostly for null safety
		 */
		ErlLoader(const char* path, ImageImpl& impl)
			: image(&impl) {
			// Open the ERL
			file.Open(path, O_RDONLY);
		}

		/**
		 * Main driver load function.
		 * \returns no error, or an ErlLoadError from any of the steps.
		 */
		LoadResult<void> Load() {
			// load the header
			auto header = LoadHeader();
			if(header.HasError())
				return header.Error();

			// Get address of the stored header,
			// and keep note of it (to avoid calling into Expected over and over)
			header_ = &(*header);

			auto sections = LoadSectionHeaders();
			if(sections.HasError())
				return sections.Error();

			shdrs_ = &(*sections);


			return NO_ERROR<ErlLoadError>;
		}

		LoadResult<Elf32_Ehdr> LoadHeader() {
			if(!file)
				return ErlLoadError::FileNotFound;

			Elf32_Ehdr header;
			if(file.Read(reinterpret_cast<void*>(&header), sizeof(header)) != sizeof(header))
				return ErlLoadError::ErrorReading;

			// Validate elf header, return error condition

			if(memcmp(header.e_ident, ELFMAG, sizeof(ELFMAG) - 1))
				return ErlLoadError::NotElf;

			if(header.e_machine != EM_MIPS)
				return ErlLoadError::NotMips;

			if(header.e_type != ET_REL)
				return ErlLoadError::NotRelocatable;

			// Do some size sanity checks

			if(sizeof(Elf32_Shdr) != header.e_shentsize)
				return ErlLoadError::SizeMismatch;

			// Now we're good.
			return header;
		}

		LoadResult<DynamicArray<Elf32_Shdr>> LoadSectionHeaders() {
			DynamicArray<Elf32_Shdr> res;
			res.Resize(header_->e_shnum);

			file.Seek(header_->e_shoff, SEEK_SET);

			if(auto count = header_->e_shnum * sizeof(Elf32_Shdr); file.Read(reinterpret_cast<void*>(&res[0]), count) != count)
				return ErlLoadError::ErrorReading;

			return res;
		}

	   private:
		util::FioFile file;
		ImageImpl* image;
		Elf32_Ehdr* header_ {};
		DynamicArray<Elf32_Shdr>* shdrs_;
	};

	// helper to reduce the boilerplate.
#define AS_IMPL() reinterpret_cast<ImageImpl*>(&this->_impl[0])
#define AS_IMPL_C() reinterpret_cast<const ImageImpl*>(&this->_impl[0])

	Image::Image() {
		constexpr auto IMPL_SIZE = sizeof(ImageImpl);
		static_assert(sizeof(_impl) >= IMPL_SIZE, "Impl data can't hold impl object");
		new(AS_IMPL()) ImageImpl();
	}

	Image::~Image() {
		AS_IMPL()->~ImageImpl();
	}

	LoadResult<void> Image::LoadFromFile(const char* filename) {
		return ErlLoader { filename, *AS_IMPL() }.Load();
	}

	Symbol Image::ResolveSymbol(const char* symbolName) {
		return AS_IMPL()->ResolveSymbol(symbolName);
	}

	const char* Image::GetFileName() const {
		return AS_IMPL_C()->GetFileName();
	}

	// may not even need this, although I think we're getting a little lucky with stack

	Image* CreateErl() {
		return new Image();
	}

	void DestroyErl(Image* theImage) {
		delete theImage;
	}

} // namespace elfldr::erl