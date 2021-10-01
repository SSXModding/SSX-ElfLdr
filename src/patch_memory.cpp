// MemClr patch - relatively useless,
// disables memory clearing done by the game.

#include <utils.h>
#include <codeutils.h>
#include <patch.h>

using namespace elfldr;

struct MemclrPatch : public Patch {
	
	void Apply() override {
		util::DebugOut("Applying MemClr patch...");
		
		// NOP fill the direct memory clearing loop in bxPreInit()
		util::NopFill<10>(util::Ptr(0x0018a6d8));
	
		// NOP fill the memory clearing logic in CMN initheapdebug(),
		// as simply NOP filling the call causes the game to crash.
		util::NopFill<32>(util::Ptr(0x0018a294));
		
#ifdef EXPERIMENTAL
		// Nop fill MEM_init() and initheapdebug from bxPreInit,
		// the experimental patch inits the memory allocator
		// BEFORE THE GAME EVEN LOADS! Fucking crazy that this works.
		util::DebugOut("Special case for Exp - killing MEM_init and initheapdebug");
		util::NopFill<6>(util::Ptr(0x0018a704));
#endif
		
		util::DebugOut("Finished applying MemClr patch...");
	}
	
};

// Register the patch into the patch system
static PatchRegistrar<MemclrPatch, 0x00> registrar;