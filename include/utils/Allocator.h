/**
 * SSX-Elfldr
 *
 * (C) 2021-2022 Lily/modeco80 <lily.modeco80@protonmail.ch>
 * under the terms of the MIT license.
 */

#ifndef ALLOCATOR_H
#define ALLOCATOR_H

#include <stddef.h>
#include <stdint.h>

// Provide C++ new/new[] and delete/delete[]
// overloads which use the Utils heap.
// Does not provide nothrow (these versions are nothrow in and of themselves)

void* operator new(size_t size);
void* operator new[](size_t size);
void operator delete(void* ptr) noexcept;
void operator delete[](void* ptr) noexcept;

// Placement new/new[]/delete/delete[]
void* operator new(size_t, void* p) noexcept;
void* operator new[](size_t, void* p) noexcept;
void operator delete(void*, void*) noexcept;
void operator delete[](void*, void*) noexcept;

namespace elfldr::util {

	// C style malloc() api
	void* Alloc(uint32_t size);
	void Free(void* ptr);

	void* AllocAligned(uint32_t size);

	void FreeAligned(void* ptr);

	/**
	 * malloc() callback type for the ERL loader.
	 */
	using Alloc_t = void* (*)(uint32_t);

	/**
	 * free() callback type for the ERL loader.
	 */
	using Free_t = void (*)(void*);

	// TODO: for multi game support, we need to probably
	// use a probe routine which does the below automatically
	// as soon as the ELF is loaded.

	/**
	 * Set the ERL loader's memory allocation/deallocation
	 * functions.
	 */
	void SetAllocationFunctions(Alloc_t alloc, Free_t free, Alloc_t alloc_aligned, Free_t free_aligned);

	/**
	 * Allocator using the global LibUtils heap.
	 * Implements the LibUtils version of the Allocator concept.
	 */
	template <class T>
	struct StdAllocator {
		using ValueType = T;
		using SizeType = size_t;

		/**
		 * Allocates storage for N * sizeof(T) objects.
		 * The memory is not constructed/does not start lifetime for all instances of T contained in it.
		 * You will have to use Allocator<T>::Construct() or pure placement-new to do so.
		 */
		[[nodiscard]] constexpr T* Allocate(size_t number) {
			ELFLDR_ASSERT(number > MaxSize());
			return static_cast<T*>(Alloc(number * sizeof(T)));
		}

		constexpr void Deallocate(T* ptr) {
			return Free(static_cast<void*>(ptr));
		}

		template <class... Args>
		constexpr void Construct(T* ptr, Args&&... args) {
			new(ptr) T(Forward<Args>(args)...);
		}

		[[nodiscard]] constexpr SizeType MaxSize() const {
			return SizeType(~0) / sizeof(T);
		}
	};

	// maybe: Allocator<const T>

} // namespace elfldr::util

#endif // ALLOCATOR_H