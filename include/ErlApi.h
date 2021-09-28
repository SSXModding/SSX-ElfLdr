#ifndef ERLAPI_H
#define ERLAPI_H

// This file defines game apis and some libc stuff in the game
// the ERL can call into, to replace the libc.
// TODO: Should this be "GameApi.h"?

#include <utils.h>
#include <codeutils.h>
//#include <structs.h>

namespace bx {
	
	template<class ...Args>
	constexpr void printf(const char* format, Args... args) {
		elfldr::util::CallFunction(elfldr::util::Ptr(0x0018ac08), format, args...);
	}
	
	// This actually calls into REAL (REwritten EA Library??) MEM_alloc and MEM_free,
	// because the REAL heap is the only good heap we can use.
	
	constexpr char* malloc(long size, const char* debugTag) {
		return elfldr::util::CallFunction<char*>(elfldr::util::Ptr(0x0023a448), "ERL-malloc", size, 0x0 /* MB_LOW */);
	}
	
	constexpr void free(char* ptr) {
		return elfldr::util::CallFunction(elfldr::util::Ptr(0x0023a998), ptr);
	}
	
	// TODO: new/delete implementations
	// which use bx::malloc/bx::free
}

#endif // ERLAPI_H