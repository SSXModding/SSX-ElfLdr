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
		
		std::uint16_t e_type;
		std::uint16_t e_machine;
		std::uint32_t e_version;
		std::uint32_t e_entry;
		std::uint32_t e_phoff;
		std::uint32_t e_shoff;
		std::uint32_t e_flags;
		std::uint16_t e_ehsize;
		std::uint16_t e_phentsize;
		std::uint16_t e_phnum;
		std::uint16_t e_shentsize;
		std::uint16_t e_shnum;
		std::uint16_t e_shstrndx;
	};

	struct elf_section_t {
		std::uint32_t sh_name;
		std::uint32_t sh_type;
		std::uint32_t sh_flags;
		std::uint32_t sh_addr; 
		std::uint32_t sh_offset;
		std::uint32_t sh_size;
		std::uint32_t sh_link;
		std::uint32_t sh_info;
		std::uint32_t sh_addralign;
		std::uint32_t sh_entsize;
	};

	struct elf_symbol_t {
		std::uint32_t st_name;
		std::uint32_t st_value;
		std::uint32_t st_size;
		std::uint8_t st_info;
		std::uint8_t st_other;
		std::uint16_t st_shndx;
	};

	struct elf_reloc_t {
		std::uint32_t r_offset;
		std::uint32_t r_info;
	};

}

#endif // ELF_H