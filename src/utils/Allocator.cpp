/**
 * SSX-Elfldr
 *
 * (C) 2021-2022 Lily/modeco80 <lily.modeco80@protonmail.ch>
 * under the terms of the MIT license.
 */

#include <utils/Allocator.h>
#include <utils/utils.h>

namespace elfldr::util {

	static Alloc_t Alloc_ptr = nullptr;
	static Free_t Free_ptr = nullptr;

	static Alloc_t Alloc_aligned_ptr = nullptr;
	static Free_t Free_aligned_ptr = nullptr;

	void* Alloc(uint32_t size) {
		ELFLDR_ASSERT(Alloc_ptr != nullptr);
		return Alloc_ptr(size);
	}

	void Free(void* ptr) {
		ELFLDR_ASSERT(Free_ptr != nullptr);
		return Free_ptr(ptr);
	}

	void* AllocAligned(uint32_t size) {
		ELFLDR_ASSERT(Alloc_aligned_ptr != nullptr);
		return Alloc_aligned_ptr(size);
	}

	void FreeAligned(void* ptr) {
		ELFLDR_ASSERT(Free_aligned_ptr != nullptr);
		return Free_aligned_ptr(ptr);
	}

	void SetAllocationFunctions(Alloc_t alloc, Free_t free, Alloc_t alloc_aligned, Free_t free_aligned) {
		Alloc_ptr = alloc;
		Free_ptr = free;
		Alloc_aligned_ptr = alloc_aligned;
		Free_aligned_ptr = free_aligned;
	}

} // namespace elfldr::util

void* operator new(size_t size) {
	auto* p = elfldr::util::Alloc(size);
	ELFLDR_VERIFY(p != nullptr && "Alloc() returned nullptr!!!");
	return p;
}

void* operator new[](size_t size) {
	auto* p = elfldr::util::Alloc(size);
	ELFLDR_VERIFY(p != nullptr && "Alloc() returned nullptr!!!");
	return p;
}

void operator delete(void* ptr) noexcept {
	if(ptr) // satisifies that delete/delete[] shouldn't break if nullptr is deleted
		elfldr::util::Free(ptr);
}

void operator delete(void* ptr, size_t) noexcept {
	if(ptr) // satisifies that delete/delete[] shouldn't break if nullptr is deleted
		elfldr::util::Free(ptr);
}

void operator delete[](void* ptr) noexcept {
	if(ptr) // satisifies that delete/delete[] shouldn't break if nullptr is deleted
		elfldr::util::Free(ptr);
}

void operator delete[](void* ptr, size_t) noexcept {
	if(ptr) // satisifies that delete/delete[] shouldn't break if nullptr is deleted
		elfldr::util::Free(ptr);
}

// placement new/delete support

void* operator new(size_t, void* p) noexcept {
	return p;
}

void* operator new[](size_t, void* p) noexcept {
	return p;
}

void operator delete(void*, void*) noexcept {
}

void operator delete[](void*, void*) noexcept {
}