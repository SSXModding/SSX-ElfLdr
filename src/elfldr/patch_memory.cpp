/**
 * SSX-Elfldr
 *
 * (C) 2021-2022 Lily/modeco80 <lily.modeco80@protonmail.ch>
 * under the terms of the MIT license.
 */

// MemClr patch - relatively useless,
// disables memory clearing done by the game.

#include <utils/codeutils.h>
#include <utils/utils.h>

#include "GameVersion.h"
#include "patch.h"

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

		if(data.region != GameRegion::NTSC)
			return false;

		if(data.game == Game::SSXOG || data.game == Game::SSXDVD)
			return true;

		return false;
	}

	void Apply() override {
		const auto& data = GetGameVersionData();

		switch(data.game) {
			case Game::SSXOG: {
				// NOP fill the direct memory clearing loop in bxPreInit()
				util::NopFill<10>(util::Ptr(0x0018a6d8));

				/*
				 * **************************************************************
				 * fuck this loop in particular                               *
				 **************************************************************
				 *	0018a6d8 0a 00 00 10  	   b          LAB_0018a704
				 */

				// util::MemRefTo<std::uint32_t>(util::Ptr(0x0018a6d8)) = 0x1000000a; // b to end of for/while loop

				// NOP fill the memory clearing logic in CMN initheapdebug(),
				// as simply NOP filling the call causes the game to crash.
				// util::NopFill<32>(util::Ptr(0x0018a294));

				util::MemRefTo<std::uint32_t>(util::Ptr(0x0018a2a0)) = 0x10000016; // b to the jr ra in initheapdebug(), this seems to be more stable on new pcsx2
				util::MemRefTo<std::uint32_t>(util::Ptr(0x0018a2a4)) = 0x00000000; // nop to make the EE happy (and avoid side-effects.)

				//#ifdef EXPERIMENTAL
				 util::DebugOut("Special case for Exp - killing MEM_init and initheapdebug");
				 util::NopFill<6>(util::Ptr(0x0018a704));
				//#endif
			} break;

			// gaming
			case Game::SSXDVD: {
				// bxPreInit
				util::MemRefTo<std::uint32_t>(util::Ptr(0x00182b08)) = 0x00000000;

				// initheapdebug()
				// nopping out the writes themselves seems to be the best here.
				util::MemRefTo<std::uint32_t>(util::Ptr(0x001826c8)) = 0x00000000;
				util::MemRefTo<std::uint32_t>(util::Ptr(0x00182700)) = 0x00000000;
			} break;

				// SSX3 release does not actually clear the memory,
				// so patch data for it isn't needed!
				// go EA
		}
	}
};

// Register the patch into the patch system
static PatchRegistrar<MemclrPatch, 0x00> registrar;