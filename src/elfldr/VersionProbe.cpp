/**
 * SSX-Elfldr
 *
 * (C) 2021-2022 Lily/modeco80 <lily.modeco80@protonmail.ch>
 * under the terms of the MIT license.
 */

#include <elfldr/GameVersion.h>
#include <fileio.h>
#include <strings.h>
#include <utils/FioDirectory.h>

// in main.cpp
extern const char* gHostFsPath;

namespace elfldr {

	void AutodetectGameVersion() {
		auto& versionData = GetGameVersionData();

		// should we then open a fd to probe the game binary version? (either by way of hashing the file or smth)
#define TryCase(game_, region_, version_, message_UNUSED_)                      \
	if(!strcasecmp(ent.name, GameBinaryFor(game_, region_, version_).CStr())) { \
		versionData.game = game_;                                               \
		versionData.region = region_;                                           \
		versionData.version = version_;                                         \
		detected = true;                                                        \
		return false;                                                           \
	}

		util::FioDirectory dir(gHostFsPath);
		bool detected = false;

		if(!dir.Ok()) {
			// Older PCSX2 versions do not support dopen() on the HostFS device,
			// for reasons that are kinda unknown to me.
			//
			// If this branch is executed, the PCSX2 version being used is probably
			// too old to work, so we intentionally crash.
			ELFLDR_VERIFY(false && "Your PCSX2 version is too old, and does not support dopen() on the HostFS device.");
		}

		dir.Iterate([&](io_dirent_t& ent) {
			if(ELFLDR_FIO_ISREG(ent)) {
				//elfldr::util::DebugOut("Regular file %s", ent.name);

				TryCase(Game::SSXOG, GameRegion::NTSC, GameVersion::SSXOG_10, "Version probe detected SSX OG (NTSC).")

				// SSXDVD
				TryCase(Game::SSXDVD, GameRegion::NTSC, GameVersion::SSXDVD_10, "Version probe detected SSX Tricky (NTSC).")

				// SSX 3
				TryCase(Game::SSX3, GameRegion::NTSC, GameVersion::SSX3_OPSM2_DEMO, "Version probe detected SSX 3 (OPSM2 Demo).")
				TryCase(Game::SSX3, GameRegion::NotApplicable, GameVersion::SSX3_KR_DEMO, "Version probe detected SSX 3 (KR Demo).")
				TryCase(Game::SSX3, GameRegion::NTSC, GameVersion::SSX3_10, "Version probe detected SSX 3 (NTSC).")
			}

			return true;
		});

#undef TryCase
		if(!detected) {
			// Mark invalid game.
			versionData.game = Game::Invalid;
		}
	}

} // namespace elfldr