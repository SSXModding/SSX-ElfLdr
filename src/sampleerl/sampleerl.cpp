// Sample ERL for Elfldr

// include the ABI header to get ABI structures.
#include <ErlAbi.h>

// This defines replacements for libc functions (printf, malloc, free) 
// inside of the bx:: namespace, which call code in the game's executable.
//
// This, while being beyond janky, saves needing to worry about linking libc,
// or needing to also load/relocate libc.erl. A Good Thing, when I only have a known
// good space of about ~350k for ERLs. In practice, this is probably more like 500k.
// 
// For memcpy, memset, and a couple other functions,
// GCC provides built-in versions of these, which should be preferred
#include <ErlApi.h>

// A sample function run every game frame,
// called by elfldr's injection of cGame::UpdateNodes().
ELFLDR_HIDDEN void SampleFunction() {
	// sorry for using windbg notation.
	bx::printf("the game is calling sample_erl!SampleFunction(), woohoo!\n");
}

// Function table
ELFLDR_HIDDEN elfldr::FunctionEntry entryTable[] {

	// sample cGame::UpdateNodes() hook
	{ 
		.type = elfldr::FunctionType::GameFrame,
		.fnPtr = &SampleFunction
	}
};

// example implementation of the only needed ERL call,
// "elfldr_get_functions"
extern "C" bool elfldr_get_functions(elfldr::ErlGetFunctionReturn* ret) {
	// if the return pointer is invalid, 
	// there's probably worse issues, lol
	if(!ret)
		return false;
	
	// Give elfldr the information it needs and then return success.
	ret->nrFunctions = sizeof(entryTable)/sizeof(entryTable[0]);
	ret->functions = &entryTable[0];
	return true;
}

// Dummy entry point.
// Code could be put here, if you *really* wanted,
// but that wouldn't really be worth it.
extern "C" int _start(int argc, char ** argv) {
	return 0;
}