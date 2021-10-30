// MemClr patch - relatively useless,
// disables memory clearing done by the game.

#include "patch.h"

#include <utils.h>
#include <codeutils.h>

#include "GameVersion.h"

using namespace elfldr;

struct MemclrPatch : public Patch {
	const char* GetName() const override {
		return "MemoryClear";
	}

	const char* GetIdentifier() const override {
		return "memclr";
	}

	bool IsCompatiable() const override {
		const auto& data = GetGameVersionData();

		if(data.game != Game::SSXOG)
			return false;

		if(data.region != GameRegion::NTSC)
			return false;

		return true;
	}

	void Apply() override {
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
	}
};

// Register the patch into the patch system
static PatchRegistrar<MemclrPatch, 0x00> registrar;