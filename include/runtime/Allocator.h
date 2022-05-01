/**
 * SSX-Elfldr
 *
 * (C) 2021-2022 Lily/modeco80 <lily.modeco80@protonmail.ch>
 * under the terms of the MIT license.
 */

#ifndef ALLOCATOR_H
#define ALLOCATOR_H

#include <runtime/Assert.h>
#include <runtime/TypeTraits.h>
#include <runtime/Utility.h>
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

namespace elfldr {

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

	using AllocFreePair = Pair<Alloc_t, Free_t>;

	// TODO: for multi game support, we need to probably
	// use a probe routine which does the below automatically
	// as soon as the ELF is loaded.

	/**
	 * Set the Runtime's memory allocation/deallocation
	 * functions.
	 *
	 * This is important, and needs to be called before
	 * any Runtime memory management (subsequently, Runtime containers)
	 * routines can be used.
	 */
	void SetAllocationFunctions(AllocFreePair memoryRoutines);

	// Allocator concept:
	//
	// template<class T>
	// struct Allocator {
	//  using ValueType = RemoveCvRefT<T>;
	//	using SizeType = appropriate_size_type; (or size_t if you're lazy)
	//
	//  // NOTE: Only allocates, does not start lifetime
	//  [[nodiscard]] constexpr ValueType* Allocate(SizeType n);
	//	constexpr void Deallocate(ValueType* p);
	//
	//	template<class... Args>
	//  constexpr void Construct(ValueType* ptr, Args&&... args);
	//
	//  [[nodiscard]] constexpr SizeType MaxSize() const;
	// };

	/**
	 * Allocator using the global runtime heap.
	 * Implements the Runtime version of the Allocator concept.
	 */
	template <class T>
	struct StdAllocator {
		using ValueType = RemoveCvRefT<T>;
		using SizeType = size_t;

		/**
		 * Allocates storage for N * sizeof(T) objects.
		 * The memory is not constructed/does not start lifetime for all instances of T contained in it.
		 * You will have to use Allocator<T>::Construct() or pure placement-new to do so.
		 */
		[[nodiscard]] constexpr ValueType* Allocate(SizeType number) {
			// ELFLDR_ASSERT(number > MaxSize());
			return static_cast<T*>(Alloc(number * sizeof(T)));
		}

		constexpr void Deallocate(ValueType* ptr) {
			return Free(static_cast<void*>(ptr));
		}

		template <class... Args>
		constexpr void Construct(ValueType* ptr, Args&&... args) {
			new(ptr) T(Forward<Args>(args)...);
		}

		[[nodiscard]] constexpr SizeType MaxSize() const {
			return SizeType(~0) / sizeof(ValueType);
		}
	};

	// maybe: Allocator<const T>

} // namespace elfldr

#endif // ALLOCATOR_H