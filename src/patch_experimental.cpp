#include "utils.h"
#include "codeutils.h"
#include "patch.h"

#include "structs.h"

// addresses of some fun stuff
constexpr static uintptr_t TheApp_Address = 0x002852f8;
constexpr static uintptr_t TheWorld_Address = 0x00299cc8;

using namespace elfldr;

struct ExpPatch : public Patch {
	
	void Apply() override {
		util::DebugOut("Applying exp patch...");
		
		// nop fill the useless loop in cGame::UpdateNodes
		util::NopFill<10>(util::Ptr(0x00189c18));
		
		// Replace the loop with a hand-written 3-instruction replacement.
		// Pre-assembled, although the assembly listing is also available.
		//
		// Instructions from 0x00189c24 to 0x00189c3c are completely fair game.
		// 0x00189c24 shouldn't modify a0 or a1, however.
		//
		// Enjoy the game loop code exec possibilities..
		// (*devilish laughter*)
		
		// The assembly:
		//
		// addiu a0, gp, 0xFFFFBDE8 ; load gNodeManager address
		// li a1, 0x3               ; load function's first parameter (3)
		// jal 0x001864b0           ; call the cNodeManager function
		
		util::MemRefTo<uint32_t>(util::Ptr(0x00189c18)) = 0x2784bde8; 
		util::MemRefTo<uint32_t>(util::Ptr(0x00189c1c)) = 0x24050003; 
		util::MemRefTo<uint32_t>(util::Ptr(0x00189c20)) = 0x0c06192c;
		
		util::DebugOut("Finished applying exp patch...");
	}
	
};

// Register the patch into the patch system
static PatchRegistrar<ExpPatch, 0xE0> registrar;