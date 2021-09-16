#include "patch.h"
#include <cassert>

// This is the max amount of patches.
// Right now this number is kinda overkill,
// but you can bump it up.
constexpr static uint32_t MAX_PATCHES = 8;

// a entry in the patch table.
struct PatchTableEntry {
	uint32_t id;
	elfldr::Patch* patch {nullptr};
};

static PatchTableEntry gPatchTable[MAX_PATCHES]{};
uint32_t gPatchTableIndex = 0;

static PatchTableEntry* FindPatchTableEntry(uint32_t id) {
	for(uint32_t i = 0; i < gPatchTableIndex; ++i) {
		if(gPatchTable[i].id == id && gPatchTable[i].patch != nullptr)
			return &gPatchTable[i];
	}
	
	return nullptr;
}

namespace elfldr {

	namespace detail {
		void RegisterPatch(uint32_t id, Patch* patch) {
			// Don't allow a null patch
			if(patch == nullptr)
				return;
			
			// Don't try and register multiple of the same ID
			if(FindPatchTableEntry(id) != nullptr)
				return;
			
			// runtime sanity check.
			assert(gPatchTableIndex < MAX_PATCHES);
			
			gPatchTable[gPatchTableIndex].id = id;
			gPatchTable[gPatchTableIndex].patch = patch;
			gPatchTableIndex++;
		}
	} // namespace detail
	
	
	Patch* GetPatchById(uint32_t id) {
		auto* entry = FindPatchTableEntry(id);
		
		if(!entry)
			return nullptr;
		
		return entry->patch;
	}
	
} // namespace elfldr