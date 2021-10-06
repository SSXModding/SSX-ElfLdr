#ifndef ALLOCATOR_H
#define ALLOCATOR_H

// This file defines 
// allocator forwarding functions
// which call into the Liberl allocation functions.
//
// This allows *everything* liberl allocates to be
// customized with the user's liberl allocation function.

namespace elfldr::erl {
	
	// C style malloc() api
	
	void* ErlAlloc(std::uint32_t size); 
	
	void ErlFree(void* ptr);
		
}

// Provide C++ new/new[] and delete/delete[]
// overloads which use the liberl heap.
// Does not provide nothrow (these versions are nothrow in and of themselves)

inline void* operator new(std::size_t size) {
	return elfldr::erl::ErlAlloc(size);
}

inline void* operator new[](std::size_t size) {
	return elfldr::erl::ErlAlloc(size);
}

inline void operator delete(void* ptr) noexcept {
	elfldr::erl::ErlFree(ptr);
}

inline void operator delete[](void* ptr) noexcept {
	elfldr::erl::ErlFree(ptr);
}


#endif // ALLOCATOR_H