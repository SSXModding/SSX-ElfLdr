#include "codeutils.h"
#include "patch.h"

struct MemclrPatch : public elfldr::Patch {
	
	void Apply() override {
		util::DebugOut("Applying MemClr patch...");
		
		// NOP fill the direct memory clearing loop in bxPreInit()
		util::NopFill<10>(reinterpret_cast<void*>(0x0018a6d8));
	
		// NOP fill the memory clearing logic in CMN initheapdebug(),
		// as simply NOP filling the call causes the game to crash.
		util::NopFill<32>(reinterpret_cast<void*>(0x0018a294));
	}
	
};
namespace elfldr {
	elfldr::Patch* GetMemoryPatch() {
		static MemclrPatch patch;
		return &patch;
	}
}