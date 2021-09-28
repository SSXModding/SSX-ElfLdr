#ifndef ERLABI_H
#define ERLABI_H

// This file defines abi types used in ERL functions
// TODO

#include <cstdint>

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
	
	struct FunctionEntry {
		FunctionType type;
		void* fnPtr;
	};
	
	// returned by the ERL's 
	// "bool elfldr_get_functions(ErlGetFunctionReturn*)" function
	struct ErlGetFunctionReturn {
		std::uint8_t nrFunctions;
		FunctionEntry* functions;
	};
	
	// the expected type of elfldr_get_functions
	using GetFunctions_t = bool(*)(ErlGetFunctionReturn*);
	
}

#endif // ERLABI_H