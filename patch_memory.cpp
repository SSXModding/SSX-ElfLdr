// MemClr patch - relatively useless,
// disables memory clearing done by the game.

#include "utils.h"
#include "codeutils.h"
#include "patch.h"

struct MemclrPatch : public elfldr::Patch {
	
	void Apply() override {
		elfldr::util::DebugOut("Applying MemClr patch...");
		
		// NOP fill the direct memory clearing loop in bxPreInit()
		elfldr::util::NopFill<10>(reinterpret_cast<void*>(0x0018a6d8));
	
		// NOP fill the memory clearing logic in CMN initheapdebug(),
		// as simply NOP filling the call causes the game to crash.
		elfldr::util::NopFill<32>(reinterpret_cast<void*>(0x0018a294));
		
		elfldr::util::DebugOut("Finished applying MemClr patch...");
	}
	
};

// Register the patch into the patch system
static elfldr::PatchRegistrar<MemclrPatch, 0x00> registrar;