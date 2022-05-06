/**
 * SSX-Elfldr SDK
 *
 * (C) 2021-2022 Lily/modeco80 <lily.modeco80@protonmail.ch>
 * under the terms of the MIT license.
 */

// ABI for the ERL layer.

#ifndef ERLABI_H
#define ERLABI_H

#include <stdint.h>

#include <utils/GameVersion.h>

//#define ELFLDR_ERL_HIDDEN __attribute__((visibility("hidden")))
#define ELFLDR_ERL_EXPORT extern "C" __attribute__((visibility("default")))

namespace elfldr {

	/**
	 * ERL ABI version. Should be bumped on any incompatible
	 * changes to any structures passed to/from ERL, especially InitErlData.
	 */
	constexpr static uint32_t ERL_ABI_VERSION = 0;

	struct InitErlData {
		util::GameVersionData verData;
		void* (*Alloc)(uint32_t);
		void (*Free)(void*);
		// any additional data. Requires an ABI bump.
	};

	// the expected type of elfldr_erl_abiversion
	using ErlAbiVersionT = uint32_t (*)();

	// the expected type of elfldr_erl_init
	using ErlInitT = void (*)(InitErlData*);

	// Declare all needed exports for a Elfldr ERL.
	// elfldr_erl_init() needs to be implemented still.
#define ELFLDR_ERL(name)                                 \
	ELFLDR_ERL_EXPORT uint32_t elfldr_erl_abiversion() { \
		return elfldr::ERL_ABI_VERSION;                  \
	}                                                    \
	ELFLDR_ERL_EXPORT int _start() {                     \
		return 0;                                        \
	}
} // namespace elfldr

#endif // ERLABI_H