#ifndef PATCH_H
#define PATCH_H

namespace elfldr {
	
	// Interface for patches.
	struct Patch {
		
		// Apply the patch.
		virtual void Apply() = 0;
		
		
		// interface to get name?
		
	};
	
	// Get the memclr patch.
	Patch* GetMemoryPatch();
	
	// Get the HostFS patch.
	Patch* GetHostFsPatch();
	
} // namespace elfldr

#endif // PATCH_H