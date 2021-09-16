#ifndef PATCH_H
#define PATCH_H

#include <cstdint>

namespace elfldr {
	
	// Interface for patches. 
	struct Patch {
		
		// Apply the patch.
		virtual void Apply() = 0;
		
		
		// interface to get name?
		
	};
	
	// only exposed for PatchRegistrar
	namespace detail {
		void RegisterPatch(uint32_t id, Patch* patch);
	} 
	
	// patch registrar, also holds a static singleton of the patch
	template<class TPatch, uint32_t ID>
	struct PatchRegistrar {
		
		PatchRegistrar() {
			detail::RegisterPatch(ID, &StaticPatch);
		}
		
		static TPatch StaticPatch;
	};
	
	template<class TPatch, uint32_t ID>
	TPatch PatchRegistrar<TPatch, ID>::StaticPatch;
	
	
	// Get a pointer to a patch's singleton by an id.
	Patch* GetPatchById(uint32_t id);
	
} // namespace elfldr

#endif // PATCH_H