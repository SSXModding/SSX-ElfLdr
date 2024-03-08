/**
 * SSX-Elfldr
 *
 * (C) 2021-2022 Lily/modeco80 <lily.modeco80@protonmail.ch>
 * under the terms of the MIT license.
 */

#include <mlstd/Assert.h>
#include <stdio.h>
#include <utils/GameVersion.h>

namespace elfldr::util {

	namespace {

		const char* SSXOGBinary(GameRegion region, GameVersion version) {
			switch(version) {
				case GameVersion::SSXOG_10:
					switch(region) {
						case GameRegion::NTSC:
							return "SLUS_200.95";
						default:
							MLSTD_ASSERT(false && "Invalid region!");
					}
					break;
			}

			MLSTD_UNREACHABLE();
		}

		const char* SSXDVDBinary(GameRegion region, GameVersion version) {
			switch(version) {
				case GameVersion::SSXDVD_10:
					switch(region) {
						case GameRegion::NTSC:
							return "SLUS_203.26";
						default:
							MLSTD_ASSERT(false && "Invalid region!");
					}
					break;
				case GameVersion::SSXDVD_JAMPACK_DEMO:
					return "SSXDEMO.ELF";

				default:
					break;
			}

			MLSTD_UNREACHABLE();
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
							MLSTD_ASSERT(false && "Invalid region!");
					}
			}

			MLSTD_UNREACHABLE();
		}

		mlstd::StringView GameToString(Game g) {
			MLSTD_ASSERT(g != Game::Invalid && "this codepath shouldn't be called with an invalid game");

			switch(g) {
				case Game::SSXOG:
					return "ssx";
				case Game::SSXDVD:
					return "ssxdvd";
				case Game::SSX3:
					return "ssx3";

				default:
					MLSTD_ASSERT(false && "Invalid game passed to GameToString()");
					return "invalid";
			}
		}

		mlstd::StringView RegionToString(GameRegion reg) {
			switch(reg) {
				case GameRegion::NotApplicable:
					return "notapplicable";
				case GameRegion::NTSC:
					return "us";
				case GameRegion::NTSCJ:
					return "jp";
				case GameRegion::PAL:
					return "eu";
			}
			MLSTD_UNREACHABLE();
		}

		mlstd::StringView VersionToString(GameVersion ver) {
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

			MLSTD_UNREACHABLE();
		}

	} // namespace

	mlstd::StringView GameBinaryFor(Game game, GameRegion region, GameVersion version) {
		switch(game) {
			case Game::SSXOG:
				return SSXOGBinary(region, version);
			case Game::SSXDVD:
				return SSXDVDBinary(region, version);
			case Game::SSX3:
				return SSX3Binary(region, version);
			default:
				MLSTD_ASSERT(false && "Invalid game passed to GameBinaryFor...");
				return "slps_000.00";
		}
	}

	mlstd::StringView GameVersionData::GetGameBinary() const {
		return GameBinaryFor(game, region, version);
	}

	mlstd::StringView GameVersionData::GameID() const {
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
