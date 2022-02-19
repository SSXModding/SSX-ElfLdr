/**
 * SSX-Elfldr SDK
 *
 * (C) 2021-2022 Lily/modeco80 <lily.modeco80@protonmail.ch>
 * under the terms of the MIT license.
 */

// Sample ERL for Elfldr.

#include <runtime/Allocator.h>
#include <sdk/ErlAbi.h>
#include <sdk/GameApi.h>
#include <utils/Hook.h>

void* (*MEM_alloc_orig)(const char*, uint64_t, int);
void (*MEM_free_orig)(void* ptr);

const char* test2 = "test data. this should work, if not, globals are busted :)";

// TODO: This should be put into another source file.
// this pretty much overrides
#ifndef NDEBUG
void __Elfldr__AssertFailure(const char* exp, const char* function, const char* file, unsigned line) {
	bx::printf("its fucked\n");
	while(true)
		;
}
#endif

void __Elfldr__VerifyFailure(const char* exp, const char* file, unsigned line) {
	bx::printf("its fucked\n");
	while(true)
		;
}

void test() {
	bx::printf("test()\n");
}

extern "C" ELFLDR_ERL_EXPORT void elfldr_erl_init(elfldr::InitErlData* erlData) {
	bx::printf("elfldr_erl_init() %s\n", test2);

	//elfldr::SetAllocationFunctions(erlData->Alloc, erlData->Free);

	//MEM_alloc_orig = elfldr::HookFunction<void* (*)(const char*, uint64_t, int)>(elfldr::util::Ptr(0x0023a448), [](const char* tag, uint64_t sz, int fl) {
	//	bx::printf("MEM_alloc: \"%s\" size %d flags %02x\n", tag, sz, fl);
	//	// allocate the memory after we yell about it
	//	return MEM_alloc_orig(tag, sz, fl);
	//});

	// MEM_free_orig = elfldr::HookFunction<void (*)(void*)>(elfldr::util::Ptr(0x0023a998), [](void* ptr) {
	//	bx::printf("MEM_free: ptr %p\n", ptr);
	//	// allocate the memory after we yell about it
	//	return MEM_free_orig(ptr);
	// });

	bx::printf("elfldr_erl_init() end\n");
}

ELFLDR_ERL("sample_erl");