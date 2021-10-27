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


// Alter this to change where HostFS root is.
#ifdef SSX3
const char* gHostFsPath = "host:C:\\pcsx2\\bin\\ssx3";
#else
//const char* gHostFsPath = "host:C:\\pcsx2\\bin\\ssxmod";

// TODO: remove the ifdef, this is how I'm handling hostfs from now on
const char* gHostFsPath = "host:";
#endif

int main() {
	elfldr::util::DebugOut("SSX-ElfLdr");
	elfldr::InitLoader();
	
	elfldr::ElfLoader loader;
	
	char elfPath[elfldr::util::MaxPath]{};
	
	// if only i had a decent FS lib.
	strncpy(&elfPath[0], gHostFsPath, elfldr::util::MaxPath * sizeof(char));
#ifdef SSX3
	strcat(elfPath, "\\SLUS_207.72");
#else
	strcat(elfPath, "SLUS_200.95");
	//strcat(elfPath, "\\SLUS_200.95");
#endif
	
	if(!loader.LoadElf(elfPath)) {
		elfldr::util::DebugOut("loader.LoadElf(%s) failed... Hanging!!", elfPath);
		while(true);
	}
	
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
	
	// reuse the elf path buffer to build the argv[0] string
	memset(&elfPath[0], 0, elfldr::util::MaxPath * sizeof(char));
	strncpy(&elfPath[0], gHostFsPath, elfldr::util::MaxPath * sizeof(char));
	//elfPath[strlen(gHostFsPath)] = '\\';
	//elfPath[strlen(gHostFsPath)+1] = '\0';
	
	// apply patches
#ifdef SSX3
	elfldr::GetPatchById(0x31)->Apply();
#else
	elfldr::GetPatchById(0x00)->Apply();
	elfldr::GetPatchById(0x01)->Apply();
#endif
	
	// apply experimental patches
#ifdef EXPERIMENTAL


	elfldr::GetPatchById(0xE0)->Apply();
#endif
	
	char* argv[1];
	
#ifndef SSX3
	argv[0] = elfPath;
#else
	argv[0] = "cdrom0:\\SLUS_207.72";
#endif
	
	// Execute the elf
	loader.ExecElf(sizeof(argv)/sizeof(argv[0]), argv);

	return 0;
}