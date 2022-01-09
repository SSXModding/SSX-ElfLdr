#include "patch.h"

#include <utils/utils.h>
#include <utils/Allocator.h>
#include <utils/codeutils.h>

#include <GameApi.h>
#include <structs.h>

#include <ErlAbi.h>

#include <erl/ErlLoader.h>

#include "GameVersion.h"

// addresses of some fun stuff
constexpr static std::uintptr_t TheApp_Address = 0x002852f8;
constexpr static std::uintptr_t TheWorld_Address = 0x00299cc8;

using namespace elfldr;

// I'm lazy.
namespace elfldr {
	void FlushCaches();
}

// aligned malloc/free for ERL

void* AlignedBxMalloc(std::uint32_t c) {
	auto raw_pointer = bx::real::MEM_alloc("Lily <3", c + sizeof(std::uint32_t), 0x40);
	auto value = reinterpret_cast<uintptr_t>(raw_pointer);
	value += (-value) & sizeof(std::uint32_t);

	// prepare the returned pointer by putting in the original malloc address
	auto* ret_pointer = reinterpret_cast<void*>(value);
	reinterpret_cast<uintptr_t*>(ret_pointer)[-1] = reinterpret_cast<uintptr_t>(raw_pointer);

	// util::DebugOut("debug(malloc): real pointer is %p", raw_pointer);
	return ret_pointer;
}

void AlignedBxFree(void* p) {
	auto real_pointer = reinterpret_cast<void*>(reinterpret_cast<uintptr_t*>(p)[-1]);
	// util::DebugOut("debug(free): real pointer is %p", real_pointer);
	if(real_pointer != nullptr)
		bx::real::MEM_free(real_pointer);
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
		util::SetAllocationFunctions([](std::uint32_t c) {
			return bx::real::MEM_alloc("Lily <3", c, 0x0 /* i forgor mbflags :( */);
		}, [](void* p) {
			if(p)
				bx::real::MEM_free(p);
		}, AlignedBxMalloc, AlignedBxFree);
		// clang-format on

		// maybe this should be a function in gameapi.h?

		// all these values are hard-coded for US. sorry.
		// You can in theory pick these values out with a little bit of guess work
		// (the game printf's out 2/3 of these, fwiw)
		constexpr static uintptr_t memstart = 0x002d9440;
		constexpr static int memsize = 30432192;

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
		util::MemRefTo<std::uint32_t>(util::Ptr(0x00189c18)) = 0x2784bde8;
		util::MemRefTo<std::uint32_t>(util::Ptr(0x00189c1c)) = 0x24050003;
		util::MemRefTo<std::uint32_t>(util::Ptr(0x00189c20)) = 0x0c06192c;

		// basic function prologue
		constexpr static std::uint32_t subroutine_prologue_template[] {
			0x27BDFFFC, // addiu sp, sp, -0x4
			0xFFB00000	// sd s0, 0x0(sp) ; save the old value of s0 as the first
		};

		constexpr static std::uint32_t subroutine_epilogue_template[] {
			// ; Restore registers to what they were before
			0xDFB00000, // ld s0, 0x0(sp) ; load the old value of s0
			0x03E00008, // jr ra
			0x27BD0004	// addiu sp, sp, 0x4 ; This instruction is executed the same time as the jr as a pipeline side effect
		};

		// Load all the erls, collect their function pointers, and then
		// get the length of said collection grouped by type

		auto* erl = erl::LoadErl("host:sample_erl.erl");
		if(erl) {
			auto sym = erl->ResolveSymbol("elfldr_get_functions");
			auto* fun = util::UBCast<GetFunctions_t>(sym);

			util::DebugOut("sym is @ %p", sym);

			if(util::UBCast<int>(sym) == -1) {
				util::DebugOut("invalid ERL!");
				erl::DestroyErl(erl);
			}

			ErlGetFunctionReturn egr{};

			if(!fun(&egr)) {
				util::DebugOut("huh?");
			} else {
				util::DebugOut("%d functions", egr.nrFunctions);
				for(int i = 0; i < egr.nrFunctions; ++i)
					util::DebugOut("type %d, ptr %08x", egr.functions[i].type, egr.functions[i].fnPtr);
			}

		}
	}
	// p[enis
};

// Register the patch into the patch system
static PatchRegistrar<ExpPatch, 0xE0> registrar;