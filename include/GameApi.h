#ifndef GAMEAPI_H
#define GAMEAPI_H

// This file defines game apis and some libc stuff in the game
// that we can call into.
//
// This header file is usable for both ERLs and the main
// ElfLdr elf.

#include <utils.h>
#include <codeutils.h>
//#include <structs.h>

namespace bx {

	constexpr static elfldr::util::VarFunction<0x0018ac08, void, const char*> printf{};
	
	// These are PS2 EE kernel system calls which we can (ab)use
	namespace eekernel {
		constexpr static elfldr::util::Function<0x002645c0, void*> EndOfHeap;
	} // namespace eekernel
	
		
	namespace real {
		constexpr static elfldr::util::Function<0x0023b2a0, void, void* /* begin */, int /* length */> MEM_init{};
		constexpr static elfldr::util::Function<0x0018a280, void, uintptr_t, uintptr_t, int> initheapdebug{};
		
		// In REAL headers, this may be char*.
		// For ease of use, it's void* here.
		
		constexpr static elfldr::util::Function<0x0023a448, void*, const char* /* label */, std::uint64_t /* size */, int /* flags */> MEM_alloc{};
		constexpr static elfldr::util::Function<0x0023a998, void, void*> MEM_free{}; 
	} // namespace real
	

	// TODO: new/delete implementations
	// which use bx::real::MEM_alloc/bx::real::MEM_free
	//
	// only for the ERL though, elfldr
	// will besides ERL allocation be zero-alloc.
}

#endif // GAMEAPI_H