#include <iopcontrol.h>
#include <iopheap.h>
#include <kernel.h>
#include <sifrpc.h>
#include <loadfile.h>
#include <fileio.h>

#include <cassert>

#include <ElfLoader.h>

#include <utils.h>
#include <codeutils.h>

namespace elfldr {

	// Global exec data, written by SifLoadElf(...)
	// We use this to run the elf in ElfLoader::ExecElf
	static t_ExecData gExecData {};

	void FlushCaches() {
		FlushCache(0);
		FlushCache(2);
	}

	bool ElfLoader::LoadElf(const char* inputPath) {
		util::DebugOut("Trying to load ELF File \"%s\"...", inputPath);

		SifLoadElf(inputPath, &gExecData);

		FlushCaches();

		return gExecData.epc > 0;
	}

	void ElfLoader::ExecElf(int argc, char** argv) {
		// this function shouldn't be called if
		// the elf didn't load properly.
		assert(gExecData.epc > 0);

		FlushCaches();

		// Reset the IOP and then ExecPS2 us.
		ResetIOP();
		ExecPS2(reinterpret_cast<void*>(gExecData.epc), reinterpret_cast<void*>(gExecData.gp), argc, argv);
	}

	void InitLoader() {
		ResetIOP();
		SifInitIopHeap();
		SifLoadFileInit();

		// Load some ROM modules.
		SifLoadModule("rom0:SIO2MAN", 0, NULL);
		SifLoadModule("rom0:MCMAN", 0, NULL);
		SifLoadModule("rom0:MCSERV", 0, NULL);
		SifLoadModule("rom0:PADMAN", 0, NULL);

		fioInit();
	}

	void ResetIOP() {
		SifInitRpc(0);

		while(!SifIopReset("", 0))
			;
		while(!SifIopSync())
			;

		SifInitRpc(0);
	}

} // namespace elfldr