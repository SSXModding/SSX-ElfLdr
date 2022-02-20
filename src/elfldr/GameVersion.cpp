/**
 * SSX-Elfldr
 *
 * (C) 2021-2022 Lily/modeco80 <lily.modeco80@protonmail.ch>
 * under the terms of the MIT license.
 */

#include <elfldr/GameVersion.h>
#include <fileio.h>
#include <runtime/Assert.h>
#include <string.h>
#include <utils/utils.h>

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

		const char* SSXOGBinary(GameRegion region, GameVersion version) {
			switch(version) {
				case GameVersion::SSXOG_10:
					switch(region) {
						case GameRegion::NTSC:
							return "SLUS_200.95";
						default:
							ELFLDR_ASSERT(false && "Invalid region!");
					}
					break;
			}

			ELFLDR_ASSERT(false);
		}

		const char* SSXDVDBinary(GameRegion region, GameVersion version) {
			switch(version) {
				case GameVersion::SSXDVD_10:
					switch(region) {
						case GameRegion::NTSC:
							return "SLUS_203.26";
						default:
							ELFLDR_ASSERT(false && "Invalid region!");
					}
					break;
			}

			ELFLDR_ASSERT(false);
		}

		const char* SSX3Binary(GameRegion region, GameVersion version) {
			switch(version) {
				case GameVersion::SSX3_OPSM2_DEMO:
					return "SSX3.ELF"; // iirc?
				case GameVersion::SSX3_KR_DEMO:
					return "SLKA_905.02";
				case GameVersion::SSX3_10:
					switch(region) {
						case GameRegion::NTSC:
							return "SLUS_207.72";
						default:
							ELFLDR_ASSERT(false && "Invalid region!");
					}
					break;
			}
		}

		const char* GameBinaryFor(Game game, GameRegion region, GameVersion version) {
			switch(game) {
				case Game::SSXOG:
					return SSXOGBinary(region, version);
				case Game::SSXDVD:
					return SSXDVDBinary(region, version);
				case Game::SSX3:
					return SSX3Binary(region, version);
				default:
					ELFLDR_VERIFY(false && "Invalid game passed to GameBinaryFor...");
			}
		}

		struct AllocAddressPair {
			void* alloc;
			void* free;

			// This isn't actually provided by the game,
			// but is a free-form lambda provided if the
			// memory allocator needs to be initialized.
			void* init;
		};

		AllocAddressPair MallocAddressFor(Game game, GameRegion region) {
		}

		void ProbeSuccess() {
			// Probe worked, let's let Runtime/Allocator know the malloc/free addresses.
		}
	} // namespace

	const char* GameVersionData::GetGameBinary() const {
		return GameBinaryFor(game, region, version);
	}

	void ProbeVersion() {
		char path[util::MaxPath] {};

#define TryCase(game_, region_, version_, message)               \
	if(TryFile(path, GameBinaryFor(game_, region_, version_))) { \
		gGameVersionData.game = game_;                           \
		gGameVersionData.region = region_;                       \
		gGameVersionData.version = version_;                     \
		util::DebugOut(message);                                 \
		return;                                                  \
	}

		TryCase(Game::SSXOG, GameRegion::NTSC, GameVersion::SSXOG_10, "Version probe detected SSX OG (NTSC).")

		// SSXDVD
		TryCase(Game::SSXDVD, GameRegion::NTSC, GameVersion::SSXDVD_10, "Version probe detected SSX Tricky (NTSC).")

		// SSX 3
		TryCase(Game::SSX3, GameRegion::NTSC, GameVersion::SSX3_OPSM2_DEMO, "Version probe detected SSX 3 (OPSM2 Demo).")
		TryCase(Game::SSX3, GameRegion::NotApplicable, GameVersion::SSX3_KR_DEMO, "Version probe detected SSX 3 (KR Demo).")
		TryCase(Game::SSX3, GameRegion::NTSC, GameVersion::SSX3_10, "Version probe detected SSX 3 (NTSC).")

#undef TryCase

		gGameVersionData.game = Game::Invalid;
	}

	const GameVersionData& GetGameVersionData() {
		return gGameVersionData;
	}

} // namespace elfldr
