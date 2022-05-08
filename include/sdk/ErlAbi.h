/**
 * SSX-Elfldr SDK
 *
 * (C) 2021-2022 Lily/modeco80 <lily.modeco80@protonmail.ch>
 * under the terms of the MIT license.
 */

// ABI for the Codehook system.

#ifndef ELFLDR_SDK_ERLABI_H
#define ELFLDR_SDK_ERLABI_H

#include <stdint.h>
#include <utils/GameVersion.h>

//#define ELFLDR_CODEHOOK_HIDDEN __attribute__((visibility("hidden")))
#define ELFLDR_CODEHOOK_EXPORT extern "C" __attribute__((visibility("default")))

namespace elfldr {

	/**
	 * ERL ABI version. Should be bumped on any incompatible
	 * changes to any structures passed to/from codehooks, especially InitErlData.
	 */
	constexpr static uint32_t CODEHOOK_ABI_VERSION = 0;

	struct CodehookInitData {
		size_t structureSize; // if this isn't equal, we've got problems.
		util::GameVersionData verData;
		// any additional data. Requires an ABI bump.
	};

	// the expected type of elfldr_codehook_abi_version
	using CodehookAbiVersionT = uint32_t (*)();

	// the expected type of elfldr_codehook_init
	using CodehookInitT = void (*)(CodehookInitData*);

	// Declare all needed exports for a Elfldr ERL.
	// elfldr_erl_init() needs to be implemented still.
#define ELFLDR_ERL(name)                                           \
	ELFLDR_CODEHOOK_EXPORT uint32_t elfldr_codehook_abiversion() { \
		return elfldr::CODEHOOK_ABI_VERSION;                       \
	}                                                              \
	ELFLDR_CODEHOOK_EXPORT int _start() {                          \
		return 0;                                                  \
	}
} // namespace elfldr

#endif // ELFLDR_SDK_ERLABI_H