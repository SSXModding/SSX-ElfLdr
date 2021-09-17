#include "patch.h"
#include "binmap.h"
#include <cassert>

// This is the max amount of patches.
// Right now this number is kinda overkill,
// but you can bump it up.
constexpr static uint32_t MAX_PATCHES = 8;

static elfldr::BinaryMap<uint32_t, elfldr::Patch*, MAX_PATCHES> gPatchMap;

namespace elfldr {

	namespace detail {
		void RegisterPatch(uint32_t id, Patch* patch) {
			// Don't allow a null patch
			if(patch == nullptr)
				return;
			
			gPatchMap.Insert(id, patch);
		}
	} // namespace detail
	
	Patch* GetPatchById(uint32_t id) {
		auto** patch = gPatchMap.MaybeGetValue(id);
		
		if(!patch)
			return nullptr;
		
		return *patch;
	}
	
} // namespace elfldr