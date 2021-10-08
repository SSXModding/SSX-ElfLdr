#include <utils.h>
#include <codeutils.h>
#include <patch.h>

#include <GameApi.h>

#include <structs.h>

// addresses of some fun stuff
constexpr static std::uintptr_t TheApp_Address = 0x002852f8;
constexpr static std::uintptr_t TheWorld_Address = 0x00299cc8;

using namespace elfldr;

// I'm lazy.
namespace elfldr { void FlushCaches(); }

struct ExpPatch : public Patch {
	
	void Apply() override {
		util::DebugOut("Applying exp patch...");
		
		// test FunctionWrappa
		bx::printf("Hello World, using the magic of FunctionWrappa<...>: %d\n", 32768);
		
		FlushCaches();
		
		bx::printf("Initalizing REAL heap, before the game does\n");
		
		// maybe this should be a function in gameapi.h?
		
		// all these values are hard-coded for US. sorry.
		// You can in theory pick these values out with a little bit of guess work
		// (the game printf's out 2/3 of these, fwiw)
		constexpr static uintptr_t memstart = 0x002d9440;
		constexpr static int memsize = 30432192;
		
		bx::real::MEM_init(util::Ptr(memstart), memsize);
		bx::real::initheapdebug(memstart, 0x002d8c20, memstart + memsize);
		
//#ifdef DEBUG		
		// Do a test allocation inside of the REAL heap,
		// to see if it works.
		void* test = bx::real::MEM_alloc("test", 320, 0x40 /* MB_LOW */);
		
		if(test != nullptr) {
			bx::printf("Wow, it actually allocated an address. It's %p\n", test);
			
			// Write a little test value in our space.
			util::MemRefTo<uint32_t>(test) = 0x13370410;
			
			bx::printf("value in test is: %d\n", util::MemRefTo<std::uint32_t>(test));
			
			// Free the memory, commented out for testing reasons.
			bx::real::MEM_free(test);
		}
//#endif
		
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
			0x27BDFFFC,  // addiu sp, sp, -0x4
			0xFFB00000   // sd s0, 0x0(sp) (save the old value of s0)
		};
		
		constexpr static std::uint32_t subroutine_epilogue_template[] {
			// Restore registers
			0xDFB00000, // ld s0, 0x0(sp) (load the old value of s0)
			0x03E00008, // jr ra
			0x27BD0004  // addiu sp, sp, 0x4 (executed as a branch delay side effect)
		};
		
	
		
		util::DebugOut("Finished applying exp patch...");
	}
	
};

// Register the patch into the patch system
static PatchRegistrar<ExpPatch, 0xE0> registrar;