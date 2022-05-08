/**
 * SSX-Elfldr
 *
 * (C) 2021-2022 Lily/modeco80 <lily.modeco80@protonmail.ch>
 * under the terms of the MIT license.
 */

#include "Patch.h"

#include "BinaryMap.h"

// This is the max amount of patches the system can take.
// Right now this number is kinda overkill,
// but you can bump it up if you're going to use
// this for something more serious.
constexpr static uint32_t MAX_PATCHES = 4;

// TODO: replace with hashmap, only allow init after memory probing done
static elfldr::BinaryMap<elfldr::PatchId, elfldr::ElfPatch*, MAX_PATCHES> gPatchMap;

namespace elfldr {

	namespace detail {
		void RegisterPatch(PatchId id, ElfPatch* patch) {
			// Don't allow a null patch
			if(patch == nullptr)
				return;

			gPatchMap.Insert(id, patch);
		}
	} // namespace detail

	ElfPatch* GetPatchById(PatchId id) {
		auto** patch = gPatchMap.MaybeGetValue(id);

		if(!patch)
			return nullptr;

		return *patch;
	}

	/*
	ElfPatch* GetPatchByIdentifier(const char* ident) {
		// ident must be a valid string pointer
		MLSTD_ASSERT(ident);

		auto** patches = gPatchMap.GetValues();

		for(int i = 0; i < gPatchMap.GetCount(); ++i)

		return nullptr;
	}
	*/

} // namespace elfldr
