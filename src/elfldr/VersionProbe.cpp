/**
 * SSX-Elfldr
 *
 * (C) 2021-2022 Lily/modeco80 <lily.modeco80@protonmail.ch>
 * under the terms of the MIT license.
 */

#include <elfldr/GameVersion.h>
#include <fileio.h>
#include <string.h>
#include <strings.h>

// in main.cpp
extern const char* gHostFsPath;

namespace elfldr {

	void AutodetectGameVersion() {
		auto& versionData = GetGameVersionData();

		// should we then open a fd to probe the game binary version? (either by way of hashing the file or smth)
#define TryCase(game_, region_, version_, message_UNUSED_)                        \
	if(!strcasecmp(entry.name, GameBinaryFor(game_, region_, version_).CStr())) { \
		versionData.game = game_;                                                 \
		versionData.region = region_;                                             \
		versionData.version = version_;                                           \
		return;                                                                   \
	}

		// utils should maybe have a FioDirectory class which handles
		// - open
		// - close
		// - iterate (with a <class DirectoryIterator>(DirectoryIterator&&)?)
		//
		// For now this is ok, if we need it elsewhere we can make it a object.

		if(auto fd = fioDopen("host:"); fd != -1) {
			io_dirent_t entry;
			while(fioDread(fd, &entry)) {
				// this is a GIANT hack but the Newlib constants don't work,
				// so we have to define our own here.

#define IOP_ISDIR(entry) (((entry).stat.mode & 0b00000001))
#define IOP_ISREG(entry) (!((entry).stat.mode & 0b00000001))

				if(IOP_ISREG(entry)) {
					// elfldr::util::DebugOut("Regular file %s", entry.name);

					TryCase(Game::SSXOG, GameRegion::NTSC, GameVersion::SSXOG_10, "Version probe detected SSX OG (NTSC).")

					// SSXDVD
					TryCase(Game::SSXDVD, GameRegion::NTSC, GameVersion::SSXDVD_10, "Version probe detected SSX Tricky (NTSC).")

					// SSX 3
					TryCase(Game::SSX3, GameRegion::NTSC, GameVersion::SSX3_OPSM2_DEMO, "Version probe detected SSX 3 (OPSM2 Demo).")
					TryCase(Game::SSX3, GameRegion::NotApplicable, GameVersion::SSX3_KR_DEMO, "Version probe detected SSX 3 (KR Demo).")
					TryCase(Game::SSX3, GameRegion::NTSC, GameVersion::SSX3_10, "Version probe detected SSX 3 (NTSC).")
				}
			}

			fioDclose(fd);
		} else {
			// Older PCSX2 versions do not support dopen() on the HostFS device, for reasons that are
			// kinda unknown to me. If this branch is executed, the PCSX2 version being used is probably
			// too old to work, so we crash.
			ELFLDR_VERIFY(false && "Your PCSX2 version is too old, and does not support dopen() on the HostFS device.");
		}

#undef TryCase

		// Mark invalid game.
		versionData.game = Game::Invalid;
	}

} // namespace elfldr