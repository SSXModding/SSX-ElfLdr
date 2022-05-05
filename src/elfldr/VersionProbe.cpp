/**
 * SSX-Elfldr
 *
 * (C) 2021-2022 Lily/modeco80 <lily.modeco80@protonmail.ch>
 * under the terms of the MIT license.
 */

#include <elfldr/GameVersion.h>
#include <utils/FioDirectory.h>

// in main.cpp
extern const char* gHostFsPath;

namespace elfldr {

	// utils please!

	template <class CharT>
	constexpr CharT CheapToLower(CharT chara) {
		if(chara >= 'A' && chara <= 'Z')
			return chara + 32;
		else
			return chara;
	}

	constexpr bool StrCaseMatch(StringView sv, StringView sv2) {
		if(sv.Length() != sv2.Length())
			return false; // Quick shortcut

		for(StringView::SizeType i = 0; i < sv.Length(); ++i)
			if(CheapToLower(sv[i]) != CheapToLower(sv2[i]))
				return false; // not matching

		return true;
	}

	void AutodetectGameVersion() {
		util::FioDirectory dir(gHostFsPath);
		bool gameDetected = false;
		auto& versionData = GetGameVersionData();

		if(!dir.Ok()) {
			// Older PCSX2 versions do not support dopen() on the HostFS device,
			// for reasons that are kinda unknown to me.
			//
			// If this branch is executed, the PCSX2 version being used is probably
			// too old to work, so we intentionally crash.
			ELFLDR_VERIFY(false && "Your PCSX2 version is too old!");
		}

		dir.Iterate([&](io_dirent_t& ent) {
			auto TryGame = [&](Game game, GameRegion region, GameVersion version) {
				if(!StrCaseMatch(ent.name, GameBinaryFor(game, region, version))) {
					versionData.game = game;
					versionData.region = region;
					versionData.version = version;
					gameDetected = true;
					return true;
				}
				return false;
			};

			if(ELFLDR_FIO_ISREG(ent)) {
				if(TryGame(Game::SSXOG, GameRegion::NTSC, GameVersion::SSXOG_10))
					return false;
				if(TryGame(Game::SSXDVD, GameRegion::NTSC, GameVersion::SSXDVD_10))
					return false;
				if(TryGame(Game::SSX3, GameRegion::NTSC, GameVersion::SSX3_OPSM2_DEMO))
					return false;
				if(TryGame(Game::SSX3, GameRegion::NotApplicable, GameVersion::SSX3_KR_DEMO))
					return false;
				if(TryGame(Game::SSX3, GameRegion::NTSC, GameVersion::SSX3_10))
					return false;
			}

			return true;
		});

		// Mark invalid game if the above loop did not detect any game.
		if(!gameDetected)
			versionData.game = Game::Invalid;
	}

} // namespace elfldr