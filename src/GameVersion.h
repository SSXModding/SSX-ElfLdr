#ifndef ELFLDR_GAMEVERSION_H
#define ELFLDR_GAMEVERSION_H

#include <cstdint>

namespace elfldr {
	
	enum class Game : std::uint8_t {
		Invalid,
		SSXOG
	};
	
	enum class GameRegion : std::uint8_t {
		NTSC,
		PAL,
		NTSCJ
	};
	
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
		
		const char* GetGameBinary() const;
	};
	
	/**
	 * Probe the game version data.
	 */
	void ProbeVersion();
	
	const GameVersionData& GetGameVersionData();
	
} // namespace elfldr

#endif // ELFLDR_GAMEVERSION_H