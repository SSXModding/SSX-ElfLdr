/**
 * SSX-Elfldr
 *
 * (C) 2021-2022 Lily/modeco80 <lily.modeco80@protonmail.ch>
 * under the terms of the MIT license.
 */

#include <erl/ErlLoader.h>
#include <runtime/Allocator.h>
#include <sdk/ErlAbi.h>
#include <sdk/GameApi.h>
#include <sdk/structs.h>
#include <utils/codeutils.h>
#include <utils/Hook.h>
#include <utils/utils.h>

#include "elfldr/GameVersion.h"
#include "patch.h"

// addresses of some fun stuff
constexpr static uintptr_t TheApp_Address = 0x002852f8;
constexpr static uintptr_t TheWorld_Address = 0x00299cc8;

static void (*cApplication_Init_Orig)(bx::cApplication* capp, char*);

using namespace elfldr;

// I'm lazy.
namespace elfldr {
	void FlushCaches();
}

struct ExpPatch : public Patch {
	const char* GetName() const override {
		return "Experimental";
	}

	const char* GetIdentifier() const override {
		return "experimental-1";
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
		FlushCaches();

		// TODO: This should be done when the elf is loaded rather than here,
		// so we can PROBABLY relax REAL stuff

		// clang-format off
		SetAllocationFunctions({ [](uint32_t c) {
			return bx::real::MEM_alloc("Lily <3", c, 0x0 /* i forgor mbflags :( */);
		}, [](void* p) {
			if(p)
				bx::real::MEM_free(p);
		}});
		// clang-format on

		// maybe this should be a function in gameapi.h?

		// all these values are hard-coded for US. sorry.
		// You can in theory pick these values out with a little bit of guess work
		// (the game printf's out 2/3 of these, fwiw)
		constexpr static uintptr_t memstart = 0x002d9440;
		constexpr static int memsize = 30432192;

		bx::printf("test\n");

		bx::real::MEM_init(util::Ptr(memstart), memsize);
		bx::real::initheapdebug(memstart, 0x002d8c20, memstart + memsize);

		// Replace the loop in cGame::UpdateNodes()
		// with a hand-written 3-instruction replacement.
		//
		// Instructions from 0x00189c24 to 0x00189c3c are completely fair game, for
		// any code you want to run during the node updating stage of cGame::Update().
		// 0x00189c24 shouldn't modify a0 or a1, however.
		//
		// Enjoy the game loop code exec possibilities..
		// (*devilish laughter*)

		// The assembly:
		//
		// addiu a0, gp, 0xFFFFBDE8 ; load gNodeManager address into a0 (this parameter) (maybe unneeded?)
		// li a1, 0x3               ; load function's first parameter into a1 (3)
		// jal 0x001864b0           ; call (linking jump) the cNodeManager function

		util::NopFill<10>(util::Ptr(0x00189c18)); // start by nopfilling

		// Put in the replacement instructions
		util::MemRefTo<uint32_t>(util::Ptr(0x00189c18)) = 0x2784bde8;
		util::MemRefTo<uint32_t>(util::Ptr(0x00189c1c)) = 0x24050003;
		util::MemRefTo<uint32_t>(util::Ptr(0x00189c20)) = 0x0c06192c;

		// basic function prologue
		constexpr static uint32_t subroutine_prologue_template[] {
			0x27BDFFFC, // addiu sp, sp, -0x4
			0xFFB00000	// sd s0, 0x0(sp) ; save the old value of s0 as the first
		};

		constexpr static uint32_t subroutine_epilogue_template[] {
			// ; Restore registers to what they were before
			0xDFB00000, // ld s0, 0x0(sp) ; load the old value of s0
			0x03E00008, // jr ra
			0x27BD0004	// addiu sp, sp, 0x4 ; This instruction is executed the same time as the jr as a pipeline side effect
		};

		// Load all the erls, collect their function pointers, and then
		// get the length of said collection grouped by type

		// Test hook
		//cApplication_Init_Orig = elfldr::HookFunction<void (*)(bx::cApplication*, char*)>(util::Ptr(0x00183a68), [](bx::cApplication* capp_this, char* p1) {
		//	bx::printf("cApplication::Init(%s) hook; orig is %p\n", p1, cApplication_Init_Orig);
		//	capp_this->mGameRate = 0.5;
		//	cApplication_Init_Orig(capp_this, p1);
		//});

#if 0
		auto* erl = erl::LoadErl("host:sample_erl.erl");

		// THIS CODE IS GENERIC!
		if(erl) {
			{
				auto sym = erl->ResolveSymbol("elfldr_erl_abiversion");
				if(!sym.IsValid()) {
					util::DebugOut("Invalid Codehook \"%s\"! (Doesn't have ABI export?)", erl->GetFileName());
					erl::DestroyErl(erl);
					return;
				}

				auto* elfldr_erl_abiversion = sym.As<uint32_t()>();
				if(elfldr_erl_abiversion() != ERL_ABI_VERSION) {
					util::DebugOut("Invalid Codehook \"%s\"! (Wrong ABI version)", erl->GetFileName());
					erl::DestroyErl(erl);
					return;
				}
			}

			// Initialize the init data.
			InitErlData ed {};
			ed.verData = GetGameVersionData();
			ed.Alloc = &Alloc;
			ed.Free = &Free;

			auto initsym = erl->ResolveSymbol("elfldr_erl_init");
			if(!initsym.IsValid()) {
				// I give up
				util::DebugOut("this erl sucks");
				erl::DestroyErl(erl);
				return;
			}

			auto elfldr_erl_init = initsym.As<void(InitErlData*)>();
			elfldr_erl_init(&ed);

			// ERL has now initialized.
		}
#endif
	}
};

// Register the patch into the patch system
static PatchRegistrar<ExpPatch, 0xE0> registrar;
