// Little tiny elf loader thing

#include "patch.h"
#include "ElfLoader.h"
#include "codeutils.h"

int main() {
	util::DebugOut("Hello world?");
	
	elfldr::ElfLoader loader;
	
	elfldr::InitLoader();
	
	if(!loader.LoadElf("host:C:\\pcsx2\\bin\\ssxmod\\SLUS_200.95")) {
		util::DebugOut("ElfLoader::LoadElf() failed... Hanging");
		while(true);
	}
	
	// apply patches
	elfldr::GetMemoryPatch()->Apply();
	elfldr::GetHostFsPatch()->Apply();
	
	char* argv[1];
	argv[0] = "host:C:\\pcsx2\\bin\\ssxmod\\";
	
	// Execute the elf
	loader.ExecElf(argv);

	return 0;
}