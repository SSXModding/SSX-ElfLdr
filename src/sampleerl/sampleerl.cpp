// Sample ERL for Elfldr

// include the ABI header to get ABI structures.
#include <ErlAbi.h>

// This defines replacements for libc functions inside of the bx:: namespace,
// which are called from the game's executable.
//
// This saves needing to worry about linking libc,
// or needing to also load/relocate libc.erl.
#include <ErlApi.h>


// A sample function run every game frame.
void SampleFunction() {
	bx::printf("SampleFunction()\n");
}

// Function table
constexpr elfldr::FunctionEntry entryTable[] {
	
	// sample cGame::UpdateNodes() hook
	{ 
		.type = elfldr::FunctionType::GameFrame,
		.fnPtr = elfldr::util::UBCast<void*>(&SampleFunction)
	}
};

// example implementation of the only needed ERL call,
// "elfldr_get_functions"
extern "C" bool elfldr_get_functions(elfldr::ErlGetFunctionReturn* ret) {
	// if the return pointer is invalid, 
	// there's probably worse issues, lol
	if(!ret)
		return false;
	
	// give elfldr the information it needs and then return success
	ret->nrFunctions = sizeof(entryTable)/sizeof(entryTable[0]);
	ret->functions = &entryTable
	return true;
}