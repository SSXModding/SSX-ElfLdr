/**
 * SSX-Elfldr
 *
 * (C) 2021-2022 Lily/modeco80 <lily.modeco80@protonmail.ch>
 * under the terms of the MIT license.
 */

#ifndef ELFLDR_GAMEVERSION_H
#define ELFLDR_GAMEVERSION_H

#include <stdint.h>

namespace elfldr {

	enum class Game : uint8_t {
		Invalid,
		SSXOG,
		SSXDVD,
		SSX3
	};

	enum class GameRegion : uint8_t {
		NTSC,
		PAL,
		NTSCJ
	};

	// THIS TYPE IS USED IN THE ERL ABI,
	// ADD FIELDS AFTER THE EXISTING ONES!!!

	/**
	 * A package of all game version data.
	 */
	struct GameVersionData {
		/**
		 * What game.
		 */
		Game game;

		/**
		 * What region.
		 */
		GameRegion region;

		// TODO: Game version?

		/**
		 * Get the game binary filename
		 */
		const char* GetGameBinary() const;
	};

	/**
	 * Probe the game version data.
	 */
	void ProbeVersion();

	const GameVersionData& GetGameVersionData();

} // namespace elfldr

#endif // ELFLDR_GAMEVERSION_H