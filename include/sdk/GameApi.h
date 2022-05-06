/**
 * SSX-Elfldr SDK
 *
 * (C) 2021-2022 Lily/modeco80 <lily.modeco80@protonmail.ch>
 * under the terms of the MIT license.
 */

#ifndef GAMEAPI_H
#define GAMEAPI_H

// This file defines game apis and some libc stuff in the game
// that we can call into, using instances of the codeutils
// function wrapper objects.
//
// This header file is usable for both ERLs and the main
// ElfLdr elf. However, any usage will have to wait until the
// ELF has been loaded, obviously. Also, these are US/SLUS-20095,
// just like all the other addresses I have.
//

#include <utils/CodeUtils.h>
#include <utils/Utils.h>
//#include <sdk/structs.h>

// TODO: dynamic function wrappers

namespace bx {

	constexpr static elfldr::util::VarFunction<0x0018ac08, void, const char*> printf {};

	// These are PS2 EE kernel system calls which we can (ab)use
	namespace eekernel {
		constexpr static elfldr::util::Function<0x002645c0, void*> EndOfHeap {};
	} // namespace eekernel

	namespace real {
		constexpr static elfldr::util::Function<0x0023b2a0, void, void* /* begin */, int /* length */> MEM_init {};
		constexpr static elfldr::util::Function<0x0018a280, void, uintptr_t, uintptr_t, int> initheapdebug {};

		// In REAL headers, this may be char*.
		// For ease of use, it's void* here.

		constexpr static elfldr::util::Function<0x0023a448, void*, const char* /* label */, uint64_t /* size */, int /* flags */> MEM_alloc {};
		constexpr static elfldr::util::Function<0x0023a998, void, void*> MEM_free {};
	} // namespace real

} // namespace bx

#endif // GAMEAPI_H