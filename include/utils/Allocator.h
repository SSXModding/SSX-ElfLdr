#ifndef ALLOCATOR_H
#define ALLOCATOR_H

// Provide C++ new/new[] and delete/delete[]
// overloads which use the Utils heap.
// Does not provide nothrow (these versions are nothrow in and of themselves)

void* operator new(std::size_t size);
void* operator new[](std::size_t size);
void operator delete(void* ptr) noexcept;
void operator delete[](void* ptr) noexcept;

// Placement new/new[]/delete/delete[]
void* operator new(std::size_t, void* p) noexcept;
void* operator new[](std::size_t, void* p) noexcept;
void operator delete(void*, void*) noexcept;
void operator delete[](void*, void*) noexcept;

namespace elfldr::util {

	// C style malloc() api
	void* Alloc(std::uint32_t size);
	void Free(void* ptr);

	void* AllocAligned(std::uint32_t size);

	void FreeAligned(void* ptr);

	/**
	 * malloc() callback type for the ERL loader.
	 */
	using Alloc_t = void* (*)(std::uint32_t);

	/**
	 * free() callback type for the ERL loader.
	 */
	using Free_t = void (*)(void*);

	/**
	 * Set the ERL loader's memory allocation/deallocation
	 * functions.
	 */
	void SetAllocationFunctions(Alloc_t alloc, Free_t free, Alloc_t alloc_aligned, Free_t free_aligned);

	// complete this later, for Array<T>, and BasicString<CharT>

	/**
	 * Allocator using new/delete. Goes on the Utils heap
	 * @tparam T
	 */
	template <class T>
	struct StdAllocator {

	};



} // namespace elfldr::util

#endif // ALLOCATOR_H