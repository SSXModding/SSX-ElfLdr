/**
 * SSX-Elfldr
 *
 * (C) 2021-2022 Lily/modeco80 <lily.modeco80@protonmail.ch>
 * under the terms of the MIT license.
 */

#include <stdint.h>
#include <string.h>
#include <utils/Hook.h>
#include <utils/MipsIEncoder.h>

namespace elfldr::detail {

	constexpr static uint32_t callTemplate[] {
		mips::nop(), // will be replaced by lui t0, top_word
		mips::nop(), // will be replaced by ori t0, t0, bot_word
		mips::jr(mips::Reg::T0),
		mips::nop() // NOP out the branch delay slot (wasting an instruction...)
	};

	void* HookFunctionBase(void* dest, const void* hook) {
		// This stack buffer temporarily holds
		// the instructions we overwrire
		uint32_t instructions[sizeof(callTemplate)];

		// No thanks.
		if(dest == nullptr || hook == nullptr)
			return nullptr;

		auto* destInstPtr = reinterpret_cast<uint32_t*>(dest);

		// Copy out the instructions
		memcpy(&instructions[0], &destInstPtr[0], sizeof(instructions) * sizeof(uint32_t));

		// Copy in the hook template
		memcpy(&destInstPtr[0], &callTemplate[0], sizeof(callTemplate) * sizeof(uint32_t));

		// Put in instructions for the funny

		destInstPtr[0] = mips::lui(mips::Reg::T0, ((uintptr_t)hook >> 16));
		destInstPtr[1] = mips::ori(mips::Reg::T0, mips::Reg::T0, (uintptr_t)hook & 0xFFFF);

		// Allocate trampoline


	}
} // namespace elfldr::detail