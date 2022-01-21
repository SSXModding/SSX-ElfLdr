/**
 * SSX-Elfldr/LibERL
 *
 * (C) 2021-2022 Lily/modeco80 <lily.modeco80@protonmail.ch>
 * under the terms of the MIT license.
 */

#ifndef ELF_H
#define ELF_H

// Internal ELF file format structures for liberl

#include <cstdint>

namespace elfldr::erl {

	// TODO: constexpr >:((((((

#define REL_TYPE 1
#define PROGBITS 1
#define NOBITS 8
#define REL 9

// New-style absoulte relocations,
// seemingly emitted now by new binutils.
#define RELA 4

#define GLOBAL 1
#define WEAK 2
#define NOTYPE 0
#define OBJECT 1
#define FUNC 2
#define SECTION 3
#define R_MIPS_32 2
#define R_MIPS_26 4
#define R_MIPS_HI16 5
#define R_MIPS_LO16 6

	using Half = std::uint16_t;
	using Addr = std::uint32_t;
	using Word = std::uint32_t;
	using SWord = std::uint32_t;
	using XWord = std::uint64_t;
	using Sxword = std::uint64_t;

	struct elf_header_t {
		union { // is there a reason why this is a union?
			std::uint8_t raw[16];
			struct e_ident_t {
				std::uint8_t ei_magic[4];
				std::uint8_t ei_class;
				std::uint8_t ei_data;
				std::uint8_t ei_version;
			} cook;
		} e_ident;

		Half e_type;
		Half e_machine;
		Word e_version;
		Word e_entry;
		Word e_phoff;
		Word e_shoff;
		Word e_flags;
		Half e_ehsize;
		Half e_phentsize;
		Half e_phnum;
		Half e_shentsize;
		Half e_shnum;
		Half e_shstrndx;
	};

	struct elf_section_t {
		Word sh_name;
		Word sh_type;
		Word sh_flags;
		Word sh_addr;
		Word sh_offset;
		Word sh_size;
		Word sh_link;
		Word sh_info;
		Word sh_addralign;
		Word sh_entsize;
	};

	struct elf_symbol_t {
		Word st_name;
		Word st_value;
		Word st_size;
		std::uint8_t st_info;
		std::uint8_t st_other;
		Half st_shndx;
	};

	struct elf_reloc_t {
		Addr r_offset;
		Word r_info;
	};

	// new style RELA relocation
	struct elf_reloca_t {
		Addr r_offset;
		Word r_info;
		Word r_addend;
	};

} // namespace elfldr::erl

#endif // ELF_H