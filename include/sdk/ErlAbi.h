/**
 * SSX-Elfldr SDK
 *
 * (C) 2021-2022 Lily/modeco80 <lily.modeco80@protonmail.ch>
 * under the terms of the MIT license.
 */

#ifndef ERLABI_H
#define ERLABI_H

// This file defines abi types used in ERL functions
// TODO

#include <cstdint>

/**
 * Declare a symbol as hidden
 */
#define ELFLDR_HIDDEN __attribute__((visibility("hidden")))

namespace elfldr {

	enum class FunctionType : std::uint8_t {
		/**
		 * Called when cGame::cGame() is called
		 */
		GameInit,

		/**
		 * Called every time cGame::UpdateNodes() is called
		 */
		GameFrame,

		/**
		 * Called every time cGame::Render() is called,
		 * so it can in theory be used to render arbitrary stuff.
		 * lmao
		 */
		GameRender
	};

	using ErlFunction_t = void (*)();

	/**
	 * a function entry.
	 * Elfldr consumes these to add to an internal list
	 * which it then emits assembler to call.
	 */
	struct FunctionEntry {
		FunctionType type;
		ErlFunction_t fnPtr;
	};

	// returned by the ERL's
	// "bool elfldr_get_functions(ErlGetFunctionReturn*)" function
	struct ErlGetFunctionReturn {
		std::uint8_t nrFunctions;
		FunctionEntry* functions;
	};

	// the expected type of elfldr_get_functions
	using GetFunctions_t = bool (*)(ErlGetFunctionReturn*);

} // namespace elfldr

#endif // ERLABI_H