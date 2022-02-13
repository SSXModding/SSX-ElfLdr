/**
 * SSX-Elfldr/LibERL
 *
 * (C) 2021-2022 Lily/modeco80 <lily.modeco80@protonmail.ch>
 * under the terms of the MIT license.
 */

#include <elfldr/ElfLoader.h>
#include <erl/ErlLoader.h>


#include <runtime/Allocator.h>
#include <runtime/Expected.h>
#include <runtime/HashTable.h>
#include <runtime/ScopeExitGuard.h>
#include <runtime/String.h>

#include <utils/codeutils.h>
#include <utils/FioFile.h>

#include "elf.h"

#define DEBUG

#define ERL_RELEASE_PRINTF(format, ...) elfldr::util::DebugOut("[ERL] " format, ##__VA_ARGS__)

// if building debug, then we can afford to be a bit more chatty.
#ifdef DEBUG
	#define ERL_DEBUG_PRINTF(format, ...) elfldr::util::DebugOut("[ERL] " format, ##__VA_ARGS__)
#else
	#define ERL_DEBUG_PRINTF(format, ...)
#endif

namespace elfldr::erl {

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
		constexpr static const char* tab[] {
			"ERL file not found",
			"Not ELF file",
			//	"Not MIPS",
			"Critical structure size mismatch",
			"Not a relocatable ELF",
			//	"No symbols",
			"Internal error relocating symbol :("
		};
		return tab[static_cast<int>(e)];
	}

	template <class T>
	using ErlResult = util::Expected<T, ErlLoadError>;

	constexpr uint32_t Align(uint32_t alignment_value, int align) {
		align--;
		if(alignment_value & align) {
			alignment_value |= align;
			alignment_value++;
		}
		return alignment_value;
	}

	// TODO: Array<Image*>
	// 	or HashTable<String(View), Image*> gLoadedImages?
	//		for dependency support

	/**
	 * This class is actually what we allocate
	 * when we give an Image* to people.
	 * It includes all the private members we might need.
	 *
	 * Also provides the implementation of Image::ResolveSymbol and image loading to begin with..
	 */
	struct ImageImpl : public Image {
		~ImageImpl() override {
			ERL_DEBUG_PRINTF("~ImageImpl()");
			delete[] bytes;
		}

		// FIXME: Refactor this function to just take the elf_reloca_t&
		// 	And/or be a template i.e <class RelocType>
		/**
		 * Apply a ELF relocation for this image.
		 */
		[[nodiscard]] bool ApplyRelocation(size_t offset, int type, uint32_t addr, uint32_t addend) const {
			uint32_t u_current_data;
			int32_t s_current_data;
			uint32_t newstate;

			if(!util::IsInstructionAligned(&bytes[offset])) {
				ERL_DEBUG_PRINTF("Unaligned relocation (at %p), type %d", &bytes[offset], type);
			}

			memcpy(&u_current_data, &bytes[offset], sizeof(uintptr_t));
			memcpy(&s_current_data, &bytes[offset], 4);

			if(addend != 0)
				addr += addend;

			switch(type) {
				case R_MIPS_32:
					newstate = s_current_data + addr;
					break;
				case R_MIPS_26:
					newstate = (u_current_data & 0xfc000000) | (((u_current_data & 0x03ffffff) + (addr >> 2)) & 0x3ffffff);
					break;
				case R_MIPS_HI16:
					newstate = (u_current_data & 0xffff0000) | ((((s_current_data << 16) >> 16) + (addr >> 16) + ((addr & 0xffff) >= 0x8000 ? 1 : 0)) & 0xffff);
					break;
				case R_MIPS_LO16:
					newstate = (u_current_data & 0xffff0000) | ((((s_current_data << 16) >> 16) + (addr & 0xffff)) & 0xffff);
					break;
				default:
					ERL_DEBUG_PRINTF("Unknown relocation type %d", type);
					return false;
			}

			memcpy(&bytes[offset], &newstate, sizeof(uint32_t));

			ERL_DEBUG_PRINTF("Changed 0x%08X data from %08X to %08X.", &bytes[offset], u_current_data, newstate);
			return true;
		}

		ErlResult<void> Load(const char* path) {
			// Stage 1: open the file.
			// If this doesn't work, then the user specified an invalid path.
			util::FioFile file;

			file.Open(path, FIO_O_RDONLY);

			if(!file)
				return ErlLoadError::FileNotFound;

			// Stage 2: Read the ELF header.
			elf_header_t header {};
			file.Read(&header, sizeof(header));

			// Check ELF signature to make sure this is actually an ELF file.
			if(header.e_ident.cook.ei_magic[0] != '\77' && header.e_ident.cook.ei_magic[1] != 'E' && header.e_ident.cook.ei_magic[2] != 'L' && header.e_ident.cook.ei_magic[3] != 'F')
				return ErlLoadError::NotElf;

			// Check if this is a relocatable ELF.
			if(header.e_type != REL_TYPE)
				return ErlLoadError::NotRelocatable;

			// NOTE: Guard for r5900 mips ELF's.
			//  ORIGINAL ERL LIBRARY DOES NOT DO THIS, so it'd be a huge improvement.

			if(sizeof(elf_section_t) != header.e_shentsize)
				return ErlLoadError::SizeMismatch;

			uint32_t fullDataSize {};

			// Let's read the section table:
			auto* sections = new elf_section_t[header.e_shnum];

			// FIXME: These should probably be elf_section_t*?
			int strtab_index {};
			int symtab_index {};
			int linked_strtab_index {};

			file.Seek(UBCast<int>(header.e_shoff), FIO_SEEK_SET);
			file.Read(sections, header.e_shnum * sizeof(elf_section_t));

			// Let's read .shstrtab:
			String shstrtab;
			shstrtab.Resize(sections[header.e_shstrndx].sh_size);

			file.Seek(UBCast<int>(sections[header.e_shstrndx].sh_offset), FIO_SEEK_SET);
			file.Read(shstrtab.data(), shstrtab.length());

			// Dump all the sections, doing some important stuff to them while doing so.
			for(Half i = 1; i < header.e_shnum; ++i) {
				auto& section = sections[i];
				StringView sectionName(&shstrtab[section.sh_name]);

				if(sectionName == ".symtab") {
					symtab_index = i;
					linked_strtab_index = UBCast<int>(section.sh_link);
				} else if(sectionName == ".strtab") {
					strtab_index = i;
				}

				if(section.sh_type == PROGBITS || section.sh_type == NOBITS) {
					fullDataSize = Align(fullDataSize,UBCast<int>(section.sh_addralign));
					section.sh_addr = fullDataSize;
					fullDataSize += section.sh_size;
				}

				ERL_DEBUG_PRINTF("Section %s: Offset 0x%08X Size 0x%08X Type %5i Link %5i Info %5i Align %5i EntSize %5i LoadAddr %08X", sectionName.CStr(), section.sh_offset, section.sh_size, section.sh_type, section.sh_link, section.sh_info, section.sh_addralign, section.sh_entsize, section.sh_addr);
			}

			// Sanity checks

			if(!symtab_index)
				return ErlLoadError::NotRelocatable;

			if(!strtab_index)
				return ErlLoadError::NotRelocatable;

			if(strtab_index != linked_strtab_index)
				return ErlLoadError::NotRelocatable;

			if(sizeof(elf_symbol_t) != sections[symtab_index].sh_entsize)
				return ErlLoadError::SizeMismatch;

			// Allocate buffer for the ERL code + data to go.
			// this->fullsize = fullDataSize;
			bytes = new uint8_t[fullDataSize];

			// Initialize the symbol hash table, so we can export symbols
			// using it.
			symbol_table.Init(64);

			ERL_RELEASE_PRINTF("ERL Base Address: 0x%08X", bytes);

			// Load in sections initially
			// PROGBITS and NOBITS
			for(int i = 1; i < header.e_shnum; ++i) {
				auto& section = sections[i];
				StringView sectionName(&shstrtab[section.sh_name]);

				switch(section.sh_type) {
					case PROGBITS:
						ERL_DEBUG_PRINTF("Reading section %s from ELF file", sectionName.CStr());
						file.Seek(UBCast<int>(section.sh_offset), FIO_SEEK_SET);
						file.Read(&bytes[section.sh_addr], section.sh_size);
						break;
					case NOBITS:
						ERL_DEBUG_PRINTF("Zeroing section %s", sectionName.CStr());
						memset(&bytes[section.sh_addr], 0x0, section.sh_size);
						break;
#ifdef DEBUG
					default:
						ERL_DEBUG_PRINTF("No action for section %s (type %d)", sectionName.CStr(), section.sh_type);
						break;
#endif
				}

				ERL_DEBUG_PRINTF("Processed section %s (at 0x%08X)", sectionName.CStr(), &bytes[section.sh_addr]);
			}

			// Load .strtab
			String strtab_names;
			strtab_names.Resize(sections[strtab_index].sh_size);

			file.Seek(UBCast<int>(sections[strtab_index].sh_offset), FIO_SEEK_SET);
			file.Read(strtab_names.data(), strtab_names.length());

			// Load .symtab
			auto* symtab = new elf_symbol_t[sections[symtab_index].sh_size / sizeof(elf_symbol_t)];

			file.Seek(UBCast<int>(sections[symtab_index].sh_offset), FIO_SEEK_SET);
			file.Read(symtab, sections[symtab_index].sh_size);

			// Load and parse the relocation section(s).

			for(int i = 0; i < header.e_shnum; ++i) {
				auto& section = sections[i];
				StringView sectionName(&shstrtab[section.sh_name]);

				if(section.sh_type != RELA)
					continue;

				// the section we'd be relocating.
				auto& relocating_section = sections[section.sh_info];

				ERL_DEBUG_PRINTF("Section %d (%s) contains rela reloc for %d (%s)", i, sectionName.CStr(), section.sh_info, StringView(&shstrtab[relocating_section.sh_name]));

				if(section.sh_entsize != sizeof(elf_reloca_t))
					return ErlLoadError::SizeMismatch;

				auto* reloc = new elf_reloca_t[(section.sh_size / section.sh_entsize)];

				// read this reloc entry into data
				file.Seek(UBCast<int>(section.sh_offset), FIO_SEEK_SET);
				file.Read(reloc, section.sh_size);

				for(Word j = 0; j < (section.sh_size / section.sh_entsize); ++j) {
					auto& r = reloc[j];
					Word symbol_number = r.r_info >> 8;
					auto& sym = symtab[symbol_number];
					auto type = UBCast<int>(r.r_info & 255);

					ERL_DEBUG_PRINTF("RelaEntry %3i: 0x%08X Type %d Addend: %d sym: %d (%s): ", j, r.r_offset, type, r.r_addend, symbol_number, StringView(&strtab_names[symtab[symbol_number].st_name]).CStr());

					switch(sym.st_info & 15) {
						case NOTYPE:
							ERL_DEBUG_PRINTF("Not handling NOTYPE for now cause it seems to be a dependent symbol thingy, and we're not doing that :)");
							break;
						case SECTION: {
							// ERL_DEBUG_PRINTF("Internal reloc to section %d strndx %d (%s)", sym, sections[sym.st_shndx].sh_name, util::StringView(&shstrtab[sections[sym.st_shndx].sh_name]).CStr());
							auto offset = relocating_section.sh_addr + r.r_offset;
							auto addr = reinterpret_cast<uintptr_t>(&bytes[sections[sym.st_shndx].sh_addr]);

							if(!ApplyRelocation(offset, type, addr, r.r_addend)) {
								ERL_DEBUG_PRINTF("Error relocating");
								// cleanup
								delete[] reloc;
								return ErlLoadError::RelocationError;
							}
						} break;
						case OBJECT:
						case FUNC:
							// TODO: Should probably implement dedupe, but whateverrrrrrrrrr
							ERL_DEBUG_PRINTF("Internal symbol relocation to %s", StringView(strtab_names.data() + sym.st_name).CStr());
							ERL_DEBUG_PRINTF("Relocating at address 0x%08X", reinterpret_cast<uintptr_t>(bytes + relocating_section.sh_addr + sym.st_value));
							if(!ApplyRelocation(relocating_section.sh_addr + sym.st_value, type, reinterpret_cast<uintptr_t>(bytes + relocating_section.sh_addr + sym.st_value), r.r_addend)) {
								ERL_DEBUG_PRINTF("Error relocating");
								// cleanup
								delete[] reloc;
								return ErlLoadError::RelocationError;
							}
							break;
						default:
							ERL_DEBUG_PRINTF("unknown relocation.");
							break;
					}
				}

				delete[] reloc;
			}

			// Let's export all symbols which should be exported.

			for(Word i = 0; i < (sections[symtab_index].sh_size / sections[symtab_index].sh_entsize); ++i) {
				if(((symtab[i].st_info >> 4) == GLOBAL) || ((symtab[i].st_info >> 4) == WEAK)) {
					if((symtab[i].st_info & 15) != NOTYPE) {
						// get stuff
						String name(&strtab_names[symtab[i].st_name]);
						auto offset = sections[symtab[i].st_shndx].sh_addr + symtab[i].st_value;
						auto address = reinterpret_cast<uintptr_t>(bytes + offset);

						ERL_RELEASE_PRINTF("Exporting ERL symbol %s @ 0x%08X", name.c_str(), address);
						symbol_table[name] = Symbol(address);
					}
				}
			}

			// ELF symbol table copy is no longer needed
			delete[] symtab;

			for(int i = 0; i < 4; ++i)
				elfldr::FlushCaches();

			// Let's call _start
			auto start = ResolveSymbol("_start").As<int()>();
			int res = start();

			// use res in release builds to avoid funny compiler warning
#ifndef DEBUG
			(void)res;
#endif
			ERL_DEBUG_PRINTF("erl's _start() returned %d", res);

			// No error occurred!
			return util::NO_ERROR<ErlLoadError>;
		}

		Symbol ResolveSymbol(const char* symbolName) override {
			// FIXME: Does this need to construct a temporary string *EVERY* time it gets called?
			//        that's a new[], memcpy(), and delete[] just to look up a symbol.
			//        Too expensive for my liking, but it shouldn't happen frequently.

			if(auto sym = symbol_table.MaybeGet(String(symbolName)); sym != nullptr) {
				return *sym;
			}

			return -1;
		}

		[[nodiscard]] const char* GetFileName() const override {
			return filename.c_str();
		}

		// Implementation data

		/**
		 * The ERL symbol table
		 */
		HashTable<String, Symbol> symbol_table;
		String filename;

		// TODO: Array<std::uint8_t>?
		uint8_t* bytes {};
		// std::uint32_t fullsize {};
	};

	Image* LoadErl(const char* path) {
		auto* image = new ImageImpl();

		// This guard frees the image in case of a load error.
		ScopeExitGuard guard([&]() {
			delete image;
		});

		ERL_DEBUG_PRINTF("Attempting to load ERL \"%s\"", path);

		image->filename = path;

		auto res = image->Load(path);
		if(res.HasError()) {
			// We know there's an error
			ERL_RELEASE_PRINTF("Error %d loading ERL \"%s\" (%s)", res.Error(), path, LoadErrorToString(res.Error()).CStr());
			return nullptr;
		}

		// OK!
		// We've loaded everything, and it's seemed to have worked!
		// Woo-hoo!
		guard.DontCall();
		return image;
	}

	void DestroyErl(Image* theImage) {
		delete theImage;
	}

} // namespace elfldr::erl