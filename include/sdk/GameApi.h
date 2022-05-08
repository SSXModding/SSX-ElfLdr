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
// This header file is used for things which can be shared across
// the SSX games. Any game-independent structures, functions, or such,
// should be left to individual game specific headers.
//
// This header file is usable for both codehooks and the main
// ModLoader elf. However, any usage will have to wait until the
// ELF has been loaded, obviously.
//

#include <utils/CodeUtils.h>
#include <utils/Utils.h>

namespace bx {

	extern elfldr::util::VarFunction<void, const char*> printf;

	// These are PS2 EE kernel system calls which we can (ab)use
	namespace eekernel {
		extern elfldr::util::Function<void*> EndOfHeap;
	} // namespace eekernel

	namespace real {
		extern elfldr::util::Function<void, void* /* begin */, int /* length */> MEM_init;
		extern elfldr::util::Function<void, uintptr_t, uintptr_t, int> initheapdebug;

		// In REAL headers, this may be char*.
		// For ease of use, it's void* here.

		extern elfldr::util::Function<void*, const char* /* label */, uint64_t /* size */, int /* flags */> MEM_alloc;
		extern elfldr::util::Function<void, void*> MEM_free;
	} // namespace real

} // namespace bx

#endif // GAMEAPI_H