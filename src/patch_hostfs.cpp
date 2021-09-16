// HostFS patch - allows game files to be loose
// on the filesystem, so it's easier to tinker with them.
//
// How this patch works is pretty simple.
// 
// 	- It first tricks the game into going into the mode where it would use "host0:",
//		by replacing the string it uses with just "host", from "cd:".
//		It also alters how many characters strncmp() will look at,
//		to make it so we don't need to supply "host0:", just "host".
//
//  - Once that's done, we write the HostFS path in a little bit of slack space,
//		and alter a pointer which usually points to a string containing "host0:"
//		to point to our slack space HostFS path string.
//
// After that, that's suprisingly all we need to do,
// and besides IOP modules needing the CD still, the rest of the game
// will load from HostFS perfectly fine, no issues.
//
// Note that (including path seperators) there is about a 31 character
// limit for the HostFS path. This techinically could be fixable by making
// the game use a slack space rather than a stack array, however, that would be
// a lot more painful, and would be a far more complicated patch. 
// A minor tradeoff to make this patch so much easier is okay, at least to me..

#include "utils.h"
#include "codeutils.h"
#include "patch.h"

// in main.cpp
extern const char* gHostFsPath;

struct HostFsPatch : public elfldr::Patch {
	
	void Apply() override {
		elfldr::util::DebugOut("Applying HostFS patch...");
		
		// where our host path string should be placed
		constexpr static uintptr_t STRING_ADDRESS = 0x002c5cc0;
		
		// where the host0 pointer is
		constexpr static uintptr_t HOST_STRING_ADDRESS = 0x002c59c8;
		
	
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
		//elfldr::util::MemRefTo<uint8_t>(reinterpret_cast<void*>(0x0023770c)) = '/';
		//elfldr::util::MemRefTo<uint8_t>(reinterpret_cast<void*>(0x00237710)) = '\\';
		
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
		
		elfldr::util::WriteString(reinterpret_cast<void*>(STRING_ADDRESS), tempPath);
		
		// Overwrite the pointer that the path "beautification" function uses to strcat()
		// "host0:" originally, pointing it to our HostFS string.
		//
		// IDK why the string isn't exactly written at 2c5cc0, so we go 4 forwards. It works.
		// I'm not questioning it
		elfldr::util::MemRefTo<uintptr_t>(reinterpret_cast<void*>(HOST_STRING_ADDRESS)) = STRING_ADDRESS + sizeof(uintptr_t);
		
		elfldr::util::DebugOut("Finished applying HostFS patch.");
	}
	
};

// Register the patch into the patch system
static elfldr::PatchRegistrar<HostFsPatch, 0x01> registrar;