// implementation tu for a couple things I found kinda bloaty
// to be inlines

#include <codeutils.h>

namespace elfldr::util {
	
	constexpr static std::uint32_t subroutine_call_template[] {
		// Load dword address to jump to
		0x3c10DEAD, // lui s0,     0xDEAD (template top word)
		0x3610BEEF, // ori s0, s0, 0xBEEF (template bottom word)
			
		// Call the loaded address.
		0x0200F809, // jalr s0
		0x00000000  // nop (to avoid branch delay mucking up s0)
	};
	
	// accessor for both words of a dword individually.
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
	
	void AddUnlimitedCallVoid(void* code, void* subroutine) {
		// TODO: actually implement this function.
		// 
		// copy the template to *code,
		// then the first 2 instructions get the top word
		// and bottom word of subroutine (presumably created via util::Ptr() or something)
	}
	
}