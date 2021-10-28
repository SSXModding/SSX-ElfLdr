#ifndef ELFLDR_GAMEVERSION_H
#define ELFLDR_GAMEVERSION_H

#include <cstdint>

namespace elfldr {
	
	enum class GameType : std::uint8_t {
		SSXOG
	};
	
	enum class GameRegion : std::uint8_t {
		NTSC,
		PAL,
		NTSCJ
	};
	
} // namespace elfldr

#endif // ELFLDR_GAMEVERSION_H