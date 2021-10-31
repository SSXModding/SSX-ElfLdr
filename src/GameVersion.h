#ifndef ELFLDR_GAMEVERSION_H
#define ELFLDR_GAMEVERSION_H

#include <cstdint>

namespace elfldr {

	enum class Game : std::uint8_t {
		Invalid,
		SSXOG,
		SSXDVD,
		SSX3
	};

	enum class GameRegion : std::uint8_t {
		NTSC,
		PAL,
		NTSCJ
	};

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