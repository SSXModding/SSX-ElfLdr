// Patching ELF loader for (right now) SSX OG
// 2021 modeco80

#include <utils.h>

#include <patch.h>
#include <ElfLoader.h>

// is this include needed?
#include <codeutils.h>

// stuff.
#include <erl/ErlLoader.h>
#include <GameApi.h>

const char* gHostFsPath = "host:";

int main() {
	elfldr::util::DebugOut("SSX-ElfLdr");
	elfldr::InitLoader();
	
	elfldr::ElfLoader loader;
	
	char elfPath[elfldr::util::MaxPath]{};
	
	// TODO: Search CWD for elfs, and identify game version.
	
	// if only i had a decent FS lib.
	strncpy(&elfPath[0], gHostFsPath, elfldr::util::MaxPath * sizeof(char));
	strcat(elfPath, "SLUS_200.95");
	
	
	ELFLDR_VERIFY(loader.LoadElf(elfPath));
	
#if 1	
	// Populate Liberl's allocation/free routines with an aligned
	// malloc/free from REAL. These SHOULDN'T be called until later though.
	elfldr::erl::SetAllocationFunctions([](std::uint32_t size) {
		return bx::real::MEM_alloc("Liberl", size + 4, 0x40 /* bx::real::MB_ALIGN */);
	}, 
	[](void* ptr) {
		if(ptr)
			bx::real::MEM_free(ptr);
	});
#endif	
	
	elfldr::GetPatchById(0x00)->Apply();
	elfldr::GetPatchById(0x01)->Apply();
	
#ifdef EXPERIMENTAL
	elfldr::GetPatchById(0xE0)->Apply();
#endif
	
	char* argv[1];
	argv[0] = elfldr::util::UBCast<char*>(gHostFsPath); // I hate this
	
	// Execute the elf
	loader.ExecElf(sizeof(argv)/sizeof(argv[0]), argv);

	return 0;
}
