/**
 * SSX-Elfldr
 *
 * (C) 2021-2022 Lily/modeco80 <lily.modeco80@protonmail.ch>
 * under the terms of the MIT license.
 */

// implementation tu for a couple things I found kinda bloaty
// to be inlines

#include <utils/codeutils.h>

namespace elfldr::util {

	// accessor type for both halves of a word individually.
	// this makes the code for AddUnlimitedCallVoid a lot less stupid
	union SeperatedWord {
		std::uint32_t Word;

		struct {
			std::uint16_t top_half;
			std::uint16_t bottom_half;
		};

		void Dump() const {
			DebugOut("SeperatedDword @ %p:", this);
			DebugOut("top half : 0x%04x", top_half);
			DebugOut("bottom half: 0x%04x", bottom_half);
		}
	};

	// This is kinda wasteful, but it should work.

	constexpr static std::uint32_t SubroutineCallTemplate[] {
		// Load dword address to jump to
		0x3c100000, // lui s0,     0x0000 (template top word)
		0x36100000, // ori s0, s0, 0x0000 (template bottom word)

		// Call the loaded address.
		0x0200F809, // jalr s0
		0x00000000	// nop (to avoid branch delay mucking up s0)
	};

	// size of the above template in bytes
	constexpr static std::size_t SubroutineCallTemplate_Size = sizeof(SubroutineCallTemplate) / sizeof(std::uint32_t);

	void ReplaceString(void* addr, const char* string) {
		DebugOut("Replacing string \"%s\" at %p: \"%s\"...", UBCast<char*>(addr), addr, string);
		__builtin_memcpy(addr, string, strlen(string) + 1);
	}

	void WriteString(void* addr, const char* string) {
		DebugOut("Writing string at %p: \"%s\"...", addr, string);
		__builtin_memcpy(addr, string, strlen(string) + 1);
	}

	void WriteRelocatableCall0(void* __restrict code, const void* __restrict subroutine) {
		auto* codeptr = UBCast<SeperatedWord*>(code);
		const auto subrdword = UBCast<SeperatedWord>(subroutine);

		// We assume the address is word aligned,
		// so we should probably keep that assumption
		ELFLDR_VERIFY(IsInstructionAligned(code));

		subrdword.Dump();
		codeptr->Dump();
		codeptr[1].Dump();

		// copy the template instructions
		__builtin_memcpy(codeptr, SubroutineCallTemplate, SubroutineCallTemplate_Size);

		// change the template values
		// to point to the subroutine
		codeptr[0].bottom_half = subrdword.top_half;
		codeptr[1].bottom_half = subrdword.bottom_half;
	}

} // namespace elfldr::util