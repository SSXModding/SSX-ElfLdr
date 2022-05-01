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
#include <strings.h>
#include <utils/utils.h>

// in main.cpp
extern const char* gHostFsPath;

// Global copy of game version data.
static elfldr::GameVersionData gGameVersionData {};

namespace elfldr {

	namespace {

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

		// should we open a fd to probe version? (either by way of hashing the file or smth)
#define TryCase(game_, region_, version_, message)                         \
	if(!strcasecmp(entry.name, GameBinaryFor(game_, region_, version_))) { \
		gGameVersionData.game = game_;                                     \
		gGameVersionData.region = region_;                                 \
		gGameVersionData.version = version_;                               \
		util::DebugOut(message);                                           \
		return;                                                            \
	}

		// utils should maybe have a FioDirectory class which handles
		// - open
		// - close
		// for now this is ok, if we need it elsewhere we can make it a object
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
		}

#undef TryCase

		gGameVersionData.game = Game::Invalid;
	}

	const GameVersionData& GetGameVersionData() {
		return gGameVersionData;
	}

} // namespace elfldr
