/**
 * SSX-Elfldr
 *
 * (C) 2021-2022 Lily/modeco80 <lily.modeco80@protonmail.ch>
 * under the terms of the MIT license.
 */

#include <sdk/GameApi.h>

namespace bx {

	elfldr::util::VarFunction<void, const char*> printf {};

	namespace eekernel {
		elfldr::util::Function<void*> EndOfHeap {};
	} // namespace eekernel

	namespace real {
		elfldr::util::Function<void, void* /* begin */, int /* length */> MEM_init {};
		elfldr::util::Function<void, uintptr_t, uintptr_t, int> initheapdebug {};
		elfldr::util::Function<void*, const char* /* label */, uint64_t /* size */, int /* flags */> MEM_alloc {};
		elfldr::util::Function<void, void*> MEM_free {};
	} // namespace real

} // namespace bx
