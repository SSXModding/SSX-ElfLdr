#ifndef ERLAPI_H
#define ERLAPI_H

// This file defines game apis and some libc stuff in the game
// the ERL can call into, to replace the libc.
// TODO: Should this be "GameApi.h"?
// This file is actually used by patch code inside of the main elf,
// so it should be gameapi.h or something

#include <utils.h>
#include <codeutils.h>
//#include <structs.h>

namespace bx {

	constexpr static elfldr::util::VaradicFunctionWrappa<void, 0x0018ac08, const char*> printf{};
	
	// These are PS2 EE kernel system calls which we can (ab)use
	namespace eekernel {
		constexpr static elfldr::util::NonVaradicFunctionWrappa<void*, 0x002645c0> EndOfHeap;
	} // namespace eekernel
	
		
	namespace real {
		constexpr static elfldr::util::NonVaradicFunctionWrappa<void, 0x0023b2a0, void* /* begin */, int /* length */> MEM_init{};
		constexpr static elfldr::util::NonVaradicFunctionWrappa<void, 0x0018a280, uintptr_t, uintptr_t, int> initheapdebug{};
		constexpr static elfldr::util::NonVaradicFunctionWrappa<void*, 0x0023a448, const char* /* label */, std::uint64_t /* size */, int /* flags */> MEM_alloc{};
		constexpr static elfldr::util::NonVaradicFunctionWrappa<void, 0x0023a998, void*> MEM_free{}; 
	} // namespace real
	

	// TODO: new/delete implementations
	// which use bx::real::MEM_alloc/bx::real::MEM_free
	//
	// only for the ERL though, elfldr
	// will besides ERL allocation be zero-alloc.
}

#endif // ERLAPI_H