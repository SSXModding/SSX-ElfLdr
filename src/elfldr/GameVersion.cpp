/**
 * SSX-Elfldr
 *
 * (C) 2021-2022 Lily/modeco80 <lily.modeco80@protonmail.ch>
 * under the terms of the MIT license.
 */

#include "GameVersion.h"

#include <fileio.h>
#include <utils/utils.h>

#include <cstring>

// in main.cpp
extern const char* gHostFsPath;

// dopen doesn't work otherwise i could do a clean iteration
// of the directory.
/*
if(auto fd = fioDopen("host:"); fd != -1) {
	io_dirent_t entry;
	while(fioDread(fd, &entry)) {
		if(entry.stat.attr & S_IFDIR)
			elfldr::util::DebugOut("Directory %s", entry.name);
		else if(entry.stat.attr & S_IFREG)
			elfldr::util::DebugOut("Regular file %s", entry.name);
	}
	fioDclose(fd);
}
*/

// Global copy of game version data.
static elfldr::GameVersionData gGameVersionData {};

// TODO: Refactor to use util::FioFile

namespace elfldr {

	namespace {
		// TODO: Maybe have this return the fd
		// if it exists, so we can do version scanning?
		bool FileExists(const char* path) {
			if(auto fd = fioOpen(path, O_RDONLY); fd > 0) {
				fioClose(fd);
				return true;
			}

			return false;
		}

		bool TryFile(char* buf, const char* path) {
			strncpy(&buf[0], gHostFsPath, elfldr::util::MaxPath * sizeof(char));
			strcat(buf, path);
			return FileExists(buf);
		}

		const char* GameBinaryFor(Game game, GameRegion region) {
			switch(game) {
				case Game::SSXOG:
					switch(region) {
						case GameRegion::NTSC:
							return "SLUS_200.95";
						case GameRegion::PAL:
							return "SLES_500.30";
						default:
							return "???";
					}
					break;
				case Game::SSXDVD:
					switch(region) {
						case GameRegion::NTSC:
							return "SLUS_203.26";
						default:
							return "???";
					}
					break;
				case Game::SSX3:
					switch(region) {
						case GameRegion::NTSC:
							return "SLUS_207.72";
						default:
							return "???";
					}
					break;
				default:
					return "???";
			}
		}
	} // namespace

	const char* GameVersionData::GetGameBinary() const {
		return GameBinaryFor(game, region);
	}

	void ProbeVersion() {
		char path[util::MaxPath] {};

#define TryCase(game_, region_, message)               \
	if(TryFile(path, GameBinaryFor(game_, region_))) { \
		gGameVersionData.game = game_;                 \
		gGameVersionData.region = region_;             \
		util::DebugOut(message);                       \
		return;                                        \
	}

		TryCase(Game::SSXOG, GameRegion::NTSC, "Version probe detected SSX OG (NTSC).")
		TryCase(Game::SSXOG, GameRegion::PAL, "Version probe detected SSX OG (PAL).")

		// SSXDVD
		TryCase(Game::SSXDVD, GameRegion::NTSC, "Version probe detected SSX Tricky (NTSC).")

		// SSX 3
		TryCase(Game::SSX3, GameRegion::NTSC, "Version probe detected SSX 3 (NTSC).")

#undef TryCase

		gGameVersionData.game = Game::Invalid;
	}

	const GameVersionData& GetGameVersionData() {
		return gGameVersionData;
	}

} // namespace elfldr