#ifndef PATCH_H
#define PATCH_H

#include <cstdint>

namespace elfldr {

	/**
	 * Abstract interface for patches to follow.
	 */
	struct Patch {
		/**
		 * Get the name of this patch.
		 */
		virtual const char* GetName() const = 0;

		// this is for configuration provisions
		virtual const char* GetIdentifier() const = 0;

		/**
		 * Get whether or not this patch is 
		 * compatiable with the current detected game.
		 *
		 * \returns true if compatiable, false otherwise
		 */
		virtual bool IsCompatiable() const = 0;

		/**
		 * Apply the patch.
		 */
		virtual void Apply() = 0;
	};

	// TODO: might be scrapping patch ids
	// in place for identifiers

	/**
	 * Patch ID type.
	 */
	using PatchId = uint8_t; // means that BinaryMap<PatchId, Patch*, N>::MapEntry = 5 bytes, rather than 8.

	// only exposed for PatchRegistrar
	namespace detail {
		void RegisterPatch(PatchId id, Patch* patch);
	}

	/**
	 * Patch registrar. Hooks a patch into the system.
	 *
	 * \tparam TPatch Patch type. Must inherit from Patch interface.
	 * \tparam ID The ID of this patch. MUST BE UNIQUE!!!!
	 */
	template <class TPatch, PatchId ID>
	struct PatchRegistrar {
		PatchRegistrar() {
			detail::RegisterPatch(ID, &StaticPatch);
		}

		static TPatch StaticPatch;
	};

	template <class TPatch, PatchId ID>
	TPatch PatchRegistrar<TPatch, ID>::StaticPatch;

	/**
	 * Get a pointer to a singleton instance of a patch,
	 * by it's patch ID.
	 *
	 * \param[in] id The ID of the patch to try and get.
	 * \returns Singleton pointer, or nullptr if invalid ID.
	 */
	Patch* GetPatchById(PatchId id);

	//Patch* GetPatchByIdentifier(const char* name);

} // namespace elfldr

#endif // PATCH_H