/**
 * SSX-Elfldr
 *
 * (C) 2021-2022 Lily/modeco80 <lily.modeco80@protonmail.ch>
 * under the terms of the MIT license.
 */

#include <runtime/Allocator.h>
#include <runtime/Assert.h>
#include <utils/utils.h>

namespace elfldr {

	static Alloc_t Alloc_ptr = nullptr;
	static Free_t Free_ptr = nullptr;

	// Right now, we only need dword alignment, so this is fine
	// if we want to do more specific alignment we can

	void* AllocAligned(uint32_t count) {
		auto raw_pointer = Alloc(count + sizeof(uint32_t));
		auto value = reinterpret_cast<uintptr_t>(raw_pointer);
		value += (-value) & sizeof(uint32_t);

		// prepare the returned pointer by putting in the original malloc address
		auto* ret_pointer = reinterpret_cast<void*>(value);
		reinterpret_cast<uintptr_t*>(ret_pointer)[-1] = reinterpret_cast<uintptr_t>(raw_pointer);

		// util::DebugOut("debug(malloc): real pointer is %p", raw_pointer);
		return ret_pointer;
	}

	void FreeAligned(void* p) {
		auto real_pointer = reinterpret_cast<void*>(reinterpret_cast<uintptr_t*>(p)[-1]);
		// util::DebugOut("debug(free): real pointer is %p", real_pointer);
		if(real_pointer != nullptr)
			Free(real_pointer);
	}

	void* Alloc(uint32_t size) {
		ELFLDR_ASSERT(Alloc_ptr != nullptr);
		return Alloc_ptr(size);
	}

	void Free(void* ptr) {
		ELFLDR_ASSERT(Free_ptr != nullptr);
		return Free_ptr(ptr);
	}

	void SetAllocationFunctions(Alloc_t alloc, Free_t free) {
		Alloc_ptr = alloc;
		Free_ptr = free;
	}

} // namespace elfldr

void* operator new(size_t size) {
	auto* p = elfldr::Alloc(size);
	ELFLDR_VERIFY(p != nullptr && "Alloc() returned nullptr!!!");
	return p;
}

void* operator new[](size_t size) {
	auto* p = elfldr::Alloc(size);
	ELFLDR_VERIFY(p != nullptr && "Alloc() returned nullptr!!!");
	return p;
}

void operator delete(void* ptr) noexcept {
	if(ptr) // satisifies that delete/delete[] shouldn't break if nullptr is deleted
		elfldr::Free(ptr);
}

void operator delete(void* ptr, size_t) noexcept {
	if(ptr) // satisifies that delete/delete[] shouldn't break if nullptr is deleted
		elfldr::Free(ptr);
}

void operator delete[](void* ptr) noexcept {
	if(ptr) // satisifies that delete/delete[] shouldn't break if nullptr is deleted
		elfldr::Free(ptr);
}

void operator delete[](void* ptr, size_t) noexcept {
	if(ptr) // satisifies that delete/delete[] shouldn't break if nullptr is deleted
		elfldr::Free(ptr);
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