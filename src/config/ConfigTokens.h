
#include <cstdint>

namespace elfldr::config {

	enum class ConfigTokenValue : std::uint16_t {
		Invalid = -1,

		Comment, // #

		LiteralKey, // ' or " (don't matter)

		Identifier, // Identifier

		GroupBegin, // {
		GroupEnd,	// }

		ArrayBegin,		 // [
		ArrayNextElement // ,
		ArrayEnd,		 // [
	}

	// example syntax/grammar:
	// # Elfldr config file
	// elfldr {
	//  game = "ssxog"
	//
	//  subgroup {
	//    subgrouped_value = 12
	//  }
	//
	//  # test
	//  value = "test"
	// }
	// global_value = 10

	struct ConfigToken {
	   private:
	}

} // namespace elfldr::config
