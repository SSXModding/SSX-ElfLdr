// Patching ELF loader for (right now) SSX OG
// 2021 modeco80

#include "utils.h"

#include "patch.h"
#include "ElfLoader.h"
#include "codeutils.h"


// Alter this to change where HostFS root is.
#ifdef SSX3
const char* gHostFsPath = "host:C:\\pcsx2\\bin\\ssx3";
#else
const char* gHostFsPath = "host:C:\\pcsx2\\bin\\ssxmod";
#endif

int main() {
	elfldr::util::DebugOut("SSX-ElfLdr");
	elfldr::InitLoader();
	
	elfldr::ElfLoader loader;
	
	char elfPath[260]{}; // MAX_PATH on Windows
	
	// if only i had a decent FS lib.
	strncpy(&elfPath[0], gHostFsPath, sizeof(elfPath)/sizeof(elfPath[0]));
#ifdef SSX3
	strcat(elfPath, "\\SLUS_207.72");
#else
	strcat(elfPath, "\\SLUS_200.95");
#endif
	
	if(!loader.LoadElf(elfPath)) {
		elfldr::util::DebugOut("loader.LoadElf(%s) failed... Hanging!!", elfPath);
		while(true);
	}
	
	// reuse the elf path buffer to build the argv[0] string
	memset(&elfPath[0], 0, sizeof(elfPath)/sizeof(elfPath[0]));
	strncpy(&elfPath[0], gHostFsPath, sizeof(elfPath)/sizeof(elfPath[0]));
	elfPath[strlen(gHostFsPath)] = '\\';
	elfPath[strlen(gHostFsPath)+1] = '\0';
	
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