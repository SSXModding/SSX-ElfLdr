/**
 * SSX-Elfldr
 *
 * (C) 2021-2022 Lily/modeco80 <lily.modeco80@protonmail.ch>
 * under the terms of the MIT license.
 */

#include <utils/FioDirectory.h>
#include <utils/GameVersion.h>

namespace elfldr::util {

	void AutodetectGameVersion() {
		util::FioDirectory dir("host:");
		bool gameDetected = false;
		auto& versionData = GetGameVersionData();

		if(!dir.Good()) {
			// Older PCSX2 versions do not support dopen() on the HostFS device,
			// for reasons that are kinda unknown to me.
			//
			// If this branch is executed, the PCSX2 version being used is probably
			// too old to work, so we intentionally crash.
			DebugOut("Your PCSX2 version is too old and doesn't support a required HostFS feature. Bailing");
			// Should really have a spin macro or an shared abort function tbh
			while(true)
				;
		}

		dir.Iterate([&](io_dirent_t& ent) {
			auto TryGame = [&](Game game, GameRegion region, GameVersion version) {
				// DebugOut("trying \"%s\"", GameBinaryFor(game, region, version).CStr());
				if(mlstd::StrCaseMatch(mlstd::StringView(ent.name), GameBinaryFor(game, region, version))) {
					versionData.game = game;
					versionData.region = region;
					versionData.version = version;
					gameDetected = true;
					return true;
				}
				return false;
			};

			// util::DebugOut("%s stat mode 0x%08x attr 0x%08x", ent.name, ent.stat.mode, ent.stat.attr);

			// This is a hack but if this works it works
			// if(ELFLDR_FIO_ISREG(ent)) {
			if(TryGame(Game::SSXOG, GameRegion::NTSC, GameVersion::SSXOG_10))
				return false;
			if(TryGame(Game::SSXDVD, GameRegion::NTSC, GameVersion::SSXDVD_10))
				return false;
			if(TryGame(Game::SSXDVD, GameRegion::NotApplicable, GameVersion::SSXDVD_JAMPACK_DEMO))
				return false;
			if(TryGame(Game::SSX3, GameRegion::NTSC, GameVersion::SSX3_OPSM2_DEMO))
				return false;
			if(TryGame(Game::SSX3, GameRegion::NotApplicable, GameVersion::SSX3_KR_DEMO))
				return false;
			if(TryGame(Game::SSX3, GameRegion::NTSC, GameVersion::SSX3_10))
				return false;
			//}

			return true;
		});

		// Mark invalid game if the above loop did not detect any game.
		if(!gameDetected)
			versionData.game = Game::Invalid;
	}

} // namespace elfldr::util