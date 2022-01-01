// implementation tu for a couple things I found kinda bloaty
// to be inlines

#include <utils/codeutils.h>

namespace elfldr::util {

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

	// accessor for both words of a dword individually.
	// this makes the code for AddUnlimitedCallVoid a lot less stupid
	union SeperatedDword {
		std::uint32_t dword;

		struct {
			std::uint16_t top_word;
			std::uint16_t bottom_word;
		};
	};

	void ReplaceString(void* addr, const char* string) {
		DebugOut("Replacing string \"%s\" at %p: \"%s\"...", UBCast<char*>(addr), addr, string);
		__builtin_memcpy(addr, string, strlen(string) + 1);
	}

	void WriteString(void* addr, const char* string) {
		DebugOut("Writing string at %p: \"%s\"...", addr, string);
		__builtin_memcpy(addr, string, strlen(string) + 1);
	}

	void WriteUnlimitedCallVoid(void* code, void* subroutine) {
		auto* codeptr = UBCast<SeperatedDword*>(code);
		const auto subrdword = UBCast<SeperatedDword>(subroutine);

		// copy the template instructions...
		__builtin_memcpy(codeptr, SubroutineCallTemplate, SubroutineCallTemplate_Size);

		// change the template's nil values
		// to each word of the subroutine.
		codeptr[0].bottom_word = subrdword.top_word;
		codeptr[1].bottom_word = subrdword.bottom_word;
	}

} // namespace elfldr::util