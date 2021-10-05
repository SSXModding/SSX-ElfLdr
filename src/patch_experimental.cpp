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
		
		// all these values are hard-coded for US. sorry.
		// You can in theory pick these values out with a little bit of guess work
		// (the game printf's out 2/3 of these, fwiw)
		constexpr static uintptr_t memstart = 0x002d9440;
		constexpr static int memsize = 30432192;

		// Init the REAL heap.
		
		bx::real::MEM_init(util::Ptr(memstart), memsize);
		bx::real::initheapdebug(memstart, 0x002d8c20, memstart + memsize);
		
		// Do a test allocation inside of the REAL heap,
		// to see if it works.
		void* test = bx::real::MEM_alloc("test", 32, 0x0 /* MB_LOW */);
		
		if(test != nullptr) {
			bx::printf("Wow, it actually allocated an address. It's %p\n", test);
			
			// Write a little test value in our space.
			util::MemRefTo<uint32_t>(test) = 0x13370410;
			
			// Free the memory, commented out for testing reasons.
			//bx::real::MEM_free(test);
		}
		
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
		
		// TODO: here is where we would insert a linking jump to code we control
		// the slack space could contain a hand-written 
		// subroutine with something like:
		//
		//  ; function epilogue
		//  addiu sp, sp, -0x4 ; reserve a dword for s0
		//	sd s0, 0x0(sp)     ; save s0 into our reserved space
		//
		//	<for each hooked function>
		//		; this is honestly a bit wasteful (4 insts/dwords per call), but,
		//		; as long as it works, it's probably fine.
		//
		//		; Load the top and bottom nibble of the function call
		//		lui s0,     0xDEAD
		//		ori s0, s0, 0xBEEF
		//
		//		jalr s0
		//		nop ; avoid ee branch delay side effects
		//	<end for each>
		//
		//	; function prologue
		//	ld s0, 0x0(sp)    ; restore s0 from the stack
		//
		//	jr ra			  ; (and then return)
		//	addiu sp, sp, 0x4 ; (and free the reserved stack)
		//
		// The code below is the pre-assembled version of these instructions.
		
		// basic function prologue
		constexpr static std::uint32_t subroutine_prologue_template[] {
			0x27BDFFFC,  // addiu sp, sp, -0x4
			0x00000000,  // nop (branch delay filler)
			0xFFB00000   // sd s0, 0x0(sp) (save the old value of s0)
		};
		
		// this is emitted for each hooked function
		constexpr static std::uint32_t subroutine_call_template[] {
			// Load dword address to jump to
			0x3c10DEAD, // lui s0,     0xDEAD (template top word)
			0x3610BEEF, // ori s0, s0, 0xBEEF (template bottom word)
			
			// Call the loaded address.
			0x0200F809, // jalr s0
			0x00000000  // nop (to avoid branch delay mucking up s0)
		};
		
		constexpr static std::uint32_t subroutine_epilogue_template[] {
			// Restore registers
			0xDFB00000, // ld s0, 0x0(sp) (load the old value of s0)
			0x00000000, // nop (branch delay filler)
			
			// Subroutine return.
			0x03E00008, // jr ra
			0x27BD0004  // addiu sp, sp, 0x4 (executed as a branch delay side effect)
		};
		
	
		
		util::DebugOut("Finished applying exp patch...");
	}
	
};

// Register the patch into the patch system
static PatchRegistrar<ExpPatch, 0xE0> registrar;