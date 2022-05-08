/**
 * SSX-Elfldr
 *
 * (C) 2021-2022 Lily/modeco80 <lily.modeco80@protonmail.ch>
 * under the terms of the MIT license.
 */

#include <elfldr/ElfLoader.h>
#include <fileio.h>
#include <iopcontrol.h>
#include <iopheap.h>
#include <kernel.h>
#include <loadfile.h>
#include <mlstd/Assert.h>
#include <sifrpc.h>
#include <utils/Utils.h>

namespace elfldr {


	void FlushCaches() {
		FlushCache(CPU_INSTRUCTION_CACHE | CPU_DATA_CACHE);
	}

	void ResetIOP() {
		SifInitRpc(0);

		while(!SifIopReset("", 0))
			;
		while(!SifIopSync())
			;

		SifInitRpc(0);
	}

	void InitLoader() {
		// Reboot the IOP.
		ResetIOP();
		SifInitIopHeap();
		SifLoadFileInit();

		// Load ROM IOP modules.
		SifLoadModule("rom0:SIO2MAN", 0, nullptr);
		SifLoadModule("rom0:MCMAN", 0, nullptr);
		SifLoadModule("rom0:MCSERV", 0, nullptr);
		SifLoadModule("rom0:PADMAN", 0, nullptr);

		// Initialize FIO.
		fioInit();
	}

	// Global exec data, written by SifLoadElf(...)
	// We use this to run the elf in ElfLoader::ExecElf
	static t_ExecData gExecData {};

	ElfLoader::ElfLoader() {
		// Initialize loader services.
		InitLoader();
	}

	bool ElfLoader::LoadElf(const char* inputPath) {
		util::DebugOut("[ElfLoader] Loading ELF File \"%s\"...", inputPath);

		SifLoadElf(inputPath, &gExecData);

		FlushCaches();

		return gExecData.epc > 0;
	}

	void ElfLoader::ExecElf(int argc, char** argv) {
		// This function shouldn't be called if
		// the elf didn't load properly.
		MLSTD_VERIFY(gExecData.epc > 0);

		FlushCaches();

		// Reset the IOP and then ExecPS2 the loaded ELF.
		ResetIOP();
		ExecPS2(reinterpret_cast<void*>(gExecData.epc), reinterpret_cast<void*>(gExecData.gp), argc, argv);
	}



} // namespace elfldr