#include "utils.h"
#include "codeutils.h"
#include "patch.h"

// temp include
#include <ErlApi.h>

#include "structs.h"

// addresses of some fun stuff
constexpr static std::uintptr_t TheApp_Address = 0x002852f8;
constexpr static std::uintptr_t TheWorld_Address = 0x00299cc8;

using namespace elfldr;

// I'm lazy.
namespace elfldr { void FlushCaches(); }

void MemInit() {

}

struct ExpPatch : public Patch {
	
	void Apply() override {
		util::DebugOut("Applying exp patch...");
		
		// test FunctionWrappa
		bx::printf("Hello World, using the magic of FunctionWrappa<...>: %d\n", 32768);
		
		FlushCaches();
		
		bx::printf("Initalizing REAL heap, before the game does\n");
		
		// all these values are hard-coded for US. sorry.
		// You can in theory pick these values out with a little bit of guess work
		// (the game printf's out 2/3 of these, fwiw)
		constexpr static uintptr_t memstart = 0x002d9440;
		constexpr static int memsize = 30432192;

		bx::real::MEM_init(util::Ptr(memstart), memsize);
		bx::real::initheapdebug(memstart, 0x002d8c20, memstart + memsize);
		
		// Do a test allocation
		
		void* test = bx::real::MEM_alloc("test", 32, 0x0 /* MB_LOW */);
		bx::printf("test is %p\n", test);
		
		// Write a little test value in our space.
		util::MemRefTo<uint32_t>(test) = 0x13370410;
		
		if(test != nullptr) {
			bx::printf("Wow, it actually allocated an address. It's %p\n", test);
			
			// Free the memory, commented out for testing reasons.
			//bx::real::MEM_free(test);
		}
		
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
		
		util::NopFill<10>(util::Ptr(0x00189c18));
		util::MemRefTo<std::uint32_t>(util::Ptr(0x00189c18)) = 0x2784bde8; 
		util::MemRefTo<std::uint32_t>(util::Ptr(0x00189c1c)) = 0x24050003; 
		util::MemRefTo<std::uint32_t>(util::Ptr(0x00189c20)) = 0x0c06192c;
		
		
		util::DebugOut("Finished applying exp patch...");
	}
	
};

// Register the patch into the patch system
static PatchRegistrar<ExpPatch, 0xE0> registrar;