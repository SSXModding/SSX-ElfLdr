// Little tiny elf loader thing

#include "utils.h"

#include "patch.h"
#include "ElfLoader.h"
#include "codeutils.h"

// Alter this to change where HostFS root is.
const char* gHostFsPath = "host:C:\\pcsx2\\bin\\ssxmod";

int main() {
	elfldr::util::DebugOut("Hello world?");
	
	elfldr::ElfLoader loader;
	
	elfldr::InitLoader();
	
	char elfPath[260]{}; // MAX_PATH on Windows
	
	// if only i had a decent FS lib.
	strncpy(&elfPath[0], gHostFsPath, sizeof(elfPath)/sizeof(elfPath[0]));
	strcat(elfPath, "\\SLUS_200.95");
	
	if(!loader.LoadElf(elfPath)) {
		elfldr::util::DebugOut("ElfLoader::LoadElf(%s) failed... Hanging", elfPath);
		while(true);
	}
	
	// reuse the elf path buffer to build the argv[0] string
	memset(&elfPath[0], 0, sizeof(elfPath)/sizeof(elfPath[0]));
	strncpy(&elfPath[0], gHostFsPath, sizeof(elfPath)/sizeof(elfPath[0]));
	elfPath[strlen(gHostFsPath)] = '\\';
	elfPath[strlen(gHostFsPath)+1] = '\0';
	
	// apply patches
	elfldr::GetMemoryPatch()->Apply();
	elfldr::GetHostFsPatch()->Apply();
	
	char* argv[1];
	argv[0] = elfPath;
	
	// Execute the elf
	loader.ExecElf(argv);

	return 0;
}