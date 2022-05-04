/**
 * SSX-Elfldr
 *
 * (C) 2021-2022 Lily/modeco80 <lily.modeco80@protonmail.ch>
 * under the terms of the MIT license.
 */

#include <elfldr/GameVersion.h>
#include <runtime/Assert.h>
#include <stdio.h>
#include <utils/utils.h>

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
			}

			ELFLDR_UNREACHABLE();
		}

		StringView GameToString(Game g) {
			ELFLDR_ASSERT(g != Game::Invalid && "this codepath shouldn't be called with an invalid game");

			switch(g) {
				case Game::SSXOG:
					return "ssx";
				case Game::SSXDVD:
					return "ssxdvd";
				case Game::SSX3:
					return "ssx3";

				default:
					ELFLDR_VERIFY(false && "how'd you get here...?");
			}
		}

		StringView RegionToString(GameRegion reg) {
			switch(reg) {
				case GameRegion::NotApplicable:
					return "na";
				case GameRegion::NTSC:
					return "us";
				case GameRegion::NTSCJ:
					return "jp";
				case GameRegion::PAL:
					return "eu";
			}
			ELFLDR_UNREACHABLE();
		}

		StringView VersionToString(GameVersion ver) {
			switch(ver) {
				case GameVersion::SSXOG_10:
				case GameVersion::SSXDVD_10:
				case GameVersion::SSX3_10:
					return "1.0";

				case GameVersion::SSXDVD_JAMPACK_DEMO:
					return "jamd";

				case GameVersion::SSX3_KR_DEMO:
					return "krd";

				case GameVersion::SSX3_OPSM2_DEMO:
					return "opmd";
			}

			ELFLDR_UNREACHABLE();
		}

	} // namespace

	StringView GameBinaryFor(Game game, GameRegion region, GameVersion version) {
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

	StringView GameVersionData::GetGameBinary() const {
		return GameBinaryFor(game, region, version);
	}

	StringView GameVersionData::GameID() const {
		static char tempBuf[128];
		int res = snprintf(&tempBuf[0], sizeof(tempBuf), "%s/%s/%s", GameToString(game).CStr(), RegionToString(region).CStr(), VersionToString(version).CStr());

		if(res == -1)
			return "";

		return { &tempBuf[0], static_cast<size_t>(res) };
	}

	GameVersionData& GetGameVersionData() {
		static GameVersionData verData;
		return verData;
	}

} // namespace elfldr
