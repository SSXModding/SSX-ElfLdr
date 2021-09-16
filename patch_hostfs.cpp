#include "utils.h"
#include "codeutils.h"
#include "patch.h"

struct HostFsPatch : public elfldr::Patch {
	
	void Apply() override {
		elfldr::util::DebugOut("Applying HostFS patch...");
		
		// ASYNCFILE_init usually gets "cd:".
		// We replace this with a string which will match "host"
		elfldr::util::ReplaceString(reinterpret_cast<void*>(0x002c4e70), "host");
		
		// Replace the strncmp length param constant in ASYNCFILE_init 
		// (by altering the instruction. Worst hack ever.)
		// from 6 to 4, so we can just use "host:".
		// This could be -= 2, but meh. This is more direct and better.
		elfldr::util::MemRefTo<uint8_t>(reinterpret_cast<void*>(0x00238550)) = 0x4;
		
		// Switch path normalization when making the "beautiful" path
		// around so it uses the Windows path seperation character
		// on linux you may actually need to remove this
		//
		// this actually isn't needed. I'm keeping it here in case it is:
		//
		//util::MemRefTo<uint8_t>(reinterpret_cast<void*>(0x0023770c)) = '/';
		//util::MemRefTo<uint8_t>(reinterpret_cast<void*>(0x00237710)) = '\\';
		
		// where our host path string should be placed
		constexpr static uintptr_t STRING_ADDRESS = 0x002c5cc0;
		
		// write a new string in some slack space.
		elfldr::util::WriteString(reinterpret_cast<void*>(STRING_ADDRESS), "host:C:\\pcsx2\\bin\\ssxmod\\");
		
		// Overwrite the pointer that the path "beautification" function uses to strcat()
		// "host0:" originally, pointing it to our HostFS string.
		//
		// IDK why the string isn't exactly written at 2c5cc0, so we go 4 forwards. It works.
		// I'm not questioning it
		elfldr::util::MemRefTo<uintptr_t>(reinterpret_cast<void*>(0x002c59c8)) = STRING_ADDRESS + sizeof(uintptr_t);
		
		elfldr::util::DebugOut("Finished pplying HostFS patch.");
	}
	
};

namespace elfldr {
		
	elfldr::Patch* GetHostFsPatch() {
		static HostFsPatch patch;
		return &patch;
	}
	
} // namespace elfldr