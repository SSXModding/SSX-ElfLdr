/**
 * SSX-Elfldr
 *
 * (C) 2021-2022 Lily/modeco80 <lily.modeco80@protonmail.ch>
 * under the terms of the MIT license.
 */

#include <stdint.h>
#include <string.h>

#include <runtime/Allocator.h>

#include <utils/Hook.h>
#include <utils/MipsIEncoder.h>

#include <kernel.h>

namespace elfldr::detail {

	constexpr static uint32_t callTemplate[] {
		mips::nop(), // will be replaced by lui t0, top_word
		mips::nop(), // will be replaced by ori t0, t0, bot_word
		mips::jr(mips::Reg::T0),
		mips::nop() // NOP out the branch delay slot (wasting an instruction...)
	};

	uint32_t* AllocTrampoline() {
		return static_cast<uint32_t*>(AllocAligned(sizeof(callTemplate) * 2));
	}

	void* HookFunctionBase(void* dest, const void* hook) {
		// Nil dest/hook are not allowed. In Release, we just don't do anything,
		// but in Debug we will hit this assert.

		ELFLDR_ASSERT(dest != nullptr || hook != nullptr);
		if(dest == nullptr || hook == nullptr)
			return nullptr;

		// Allocate aligned memory for the trampoline.
		// This memory is allocated before we do anything with the function,
		// so hooking the allocator is doable.
		auto* trampolineBuf = AllocTrampoline();
		auto* destInstPtr = reinterpret_cast<uint32_t*>(dest);

		// Copy out the instructions from the original function,
		// into our safekeeping buffer.
		// Then, copy the call template into the function.
		memcpy(&trampolineBuf[0], &destInstPtr[0], sizeof(callTemplate));
		memcpy(&destInstPtr[0], &callTemplate[0], sizeof(callTemplate));

		// Place the instructions to load the hook address.
		destInstPtr[0] = mips::lui(mips::Reg::T0, ((uintptr_t)hook >> 16));
		destInstPtr[1] = mips::ori(mips::Reg::T0, mips::Reg::T0, (uintptr_t)hook & 0xFFFF);

		//ELFLDR_VERIFY(trampolineBuf != nullptr && "Failed to allocate trampoline buffer.");

		memcpy(&trampolineBuf[sizeof(callTemplate)/sizeof(uint32_t)], &callTemplate[0], sizeof(callTemplate));

		// Calculate the start of the original function's instructions.
		uintptr_t tramp_dest = (uintptr_t)dest + (sizeof(callTemplate));

		trampolineBuf[sizeof(callTemplate)/sizeof(uint32_t) ] = mips::lui(mips::Reg::T0, tramp_dest >> 16);
		trampolineBuf[sizeof(callTemplate)/sizeof(uint32_t) + 1] = mips::ori(mips::Reg::T0, mips::Reg::T0, tramp_dest & 0xFFFF);

		// Flush D/I cache, just in case.
		FlushCache(CPU_DATA_CACHE);
		FlushCache(CPU_INSTRUCTION_CACHE);

		return trampolineBuf;
	}

	// definitely enough information to implement a Unhook routine,
	// just pass in the function + tramp ptr, copy the original tramp bytes
	// back to the function, free it, bam. unhooked

} // namespace elfldr::detail