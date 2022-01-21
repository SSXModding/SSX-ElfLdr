/**
 * SSX-Elfldr SDK
 *
 * (C) 2021-2022 Lily/modeco80 <lily.modeco80@protonmail.ch>
 * under the terms of the MIT license.
 */

// Sample ERL for Elfldr

// include the ABI header to get ABI structures.
#include <sdk/ErlAbi.h>

// This defines replacements for libc functions (printf, malloc, free)
// inside of the bx:: namespace, which call code inside of
// the game's executable automatically.
//
// This, while being beyond janky, saves needing to worry about linking libc
// with the ERL,
// or needing to also load/relocate libc.erl. A Good Thing, even though
// we probably *could*.
//
// For memcpy, memset, and a couple other functions,
// GCC provides built-in versions of these, which should be preferred, or
// I'll provide a "mini" runtime in the SDK for it.
#include <sdk/GameApi.h>

// A sample function run every game frame,
// called by elfldr's injection of cGame::UpdateNodes().
ELFLDR_HIDDEN void SampleFunction() {
	bx::printf("sample_erl!SampleFunction()");
}

// Function table
ELFLDR_HIDDEN elfldr::FunctionEntry entryTable[] {
	// clang-format off

	// sample cGame::UpdateNodes() hook
	{
		.type = elfldr::FunctionType::GameFrame,
		.fnPtr = &SampleFunction
	}

	// clang-format on
};

// example implementation of the only needed ERL call,
// "elfldr_get_functions". You don't really need to change this,
// unless you wanna do some stuff before Elfldr knows your stuff.

extern "C" bool elfldr_get_functions(elfldr::ErlGetFunctionReturn* ret) {
	bx::printf("elfldr_get_functions(ret: %p)", ret);
	bx::printf("function is @ %p", entryTable[0].fnPtr);

	// If the return pointer is invalid,
	// there's probably worse issues.
	if(!ret)
		return false;

	// Give elfldr the information it needs and then return success.
	ret->nrFunctions = sizeof(entryTable) / sizeof(entryTable[0]);
	ret->functions = reinterpret_cast<elfldr::FunctionEntry*>(&entryTable);
	return true;
}

extern "C" int _start() {
	bx::printf("Hi, world?\n");
	return 0;
}