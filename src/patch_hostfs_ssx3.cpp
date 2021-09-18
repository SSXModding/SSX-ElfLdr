// HostFS patch (SSX3 version) - allows game files to be loose
// on the filesystem, so it's easier to tinker with them.
//
// How this patch works is pretty simple.
// 
//	- It first tricks the game into going into the mode where it would use "host0:",
//		by replacing the string it uses with just "host", from "cd:".
//		It also alters how many characters strncmp() will look at,
//		to make it so we don't need to supply "host0:", just "host".
//
//	- Once that's done, we write the HostFS path in a little bit of slack space,
//		and alter a pointer which usually points to a string containing "host0:"
//		to point to our slack space HostFS path string.
//
// After that, that's suprisingly all we need to do,
// and besides IOP modules needing the CD still, the rest of the game
// will load from HostFS perfectly fine, no issues.
//
// Unlike the SSX OG version of this patch, SSX3's buffer size for it's
// path beautification is a massive 256 characters, so there's not as much
// of a limit.

// currently doesn't work

#include "utils.h"
#include "codeutils.h"
#include "patch.h"

// in main.cpp
extern const char* gHostFsPath;

struct HostFsPatchSSX3 : public elfldr::Patch {
	
	void Apply() override {
		elfldr::util::DebugOut("Applying HostFS (SSX3) patch...");
		
		// where our host path string should be placed
		constexpr static uintptr_t STRING_ADDRESS = 0x0047fdb0;
		
		// where the host0 pointer is
		constexpr static uintptr_t HOST_POINTER_ADDRESS = 0x00450c50;
		
	
		// ASYNCFILE_init usually gets "cd:".
		// We replace this with a string which will match "host"
		
		elfldr::util::ReplaceString(reinterpret_cast<void*>(0x004a3ed8), "ho");
		
		// for some reason this fucking crashes the game, i don't get why
		//elfldr::util::MemRefTo<uint32_t>(reinterpret_cast<void*>(0x004a3ed8)) = 0x003A6F68; // "ho:\0"
		
		// throw the strlen() result away in an instruction meant to put it
		// into the register compiler allocated, and replace it with a minimally viable constant
		elfldr::util::MemRefTo<uint32_t>(reinterpret_cast<void*>(0x003ddea0)) = 0x02001124; // li ...(I forget), 2
				
		// write a new string in some slack space.
		
		// Assemble a good path string from the global HostFS path,
		// by copying it into a temporary buffer and then adding an extra
		// path seperator.
		char tempPath[260]{};
		
		strncpy(&tempPath[0], gHostFsPath, sizeof(tempPath)/sizeof(tempPath[0]));
		tempPath[strlen(gHostFsPath)] = '\\';
		tempPath[strlen(gHostFsPath)+1] = '\0';
		
		// honestly this might not be needed, because the path seems to be affected
		// more by argv[0]. I'm still gonna keep it though just to be sure.
		
		//elfldr::util::WriteString(reinterpret_cast<void*>(STRING_ADDRESS), tempPath);
		
		// Overwrite the pointer that the path "beautification" function uses to strcat()
		// "host0:" originally, pointing it to our HostFS string.
		//
		// IDK why the string isn't exactly written at 2c5cc0, so we go 4 forwards. It works.
		// I'm not questioning it
		//elfldr::util::MemRefTo<uintptr_t>(reinterpret_cast<void*>(HOST_POINTER_ADDRESS)) = STRING_ADDRESS + sizeof(uintptr_t);
		
		elfldr::util::DebugOut("Finished applying HostFS patch.");
	}
	
};

// Register the patch into the patch system
static elfldr::PatchRegistrar<HostFsPatchSSX3, 0x31> registrar;