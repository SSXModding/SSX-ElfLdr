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

#include "GameVersion.h"

const char* gHostFsPath = "host:";
elfldr::ElfLoader gLoader;

void DoLoadElf(const elfldr::GameVersionData& gdata) {
	char elfPath[elfldr::util::MaxPath] {};
	strncpy(&elfPath[0], gHostFsPath, elfldr::util::MaxPath * sizeof(char));
	strcat(elfPath, gdata.GetGameBinary());

	ELFLDR_VERIFY(gLoader.LoadElf(elfPath));
}

int main() {
	elfldr::util::DebugOut("SSX-ElfLdr");

	// Init loader services.
	elfldr::InitLoader();

	elfldr::util::DebugOut("Probing game version...");
	elfldr::ProbeVersion();

	const auto& gdata = elfldr::GetGameVersionData();

	if(gdata.game == elfldr::Game::Invalid) {
		elfldr::util::DebugOut("No game that is supported could be detected alongside ElfLdr.");
		elfldr::util::DebugOut("Make sure elfldr is in the proper spot.");
		ELFLDR_VERIFY(false);
	}

	DoLoadElf(gdata);

	elfldr::GetPatchById(0x00)->Apply();
	elfldr::GetPatchById(0x01)->Apply();

#ifdef EXPERIMENTAL
	elfldr::GetPatchById(0xE0)->Apply();
#endif

	char* argv[1];
	argv[0] = elfldr::util::UBCast<char*>(gHostFsPath); // I hate this

	// Execute the elf
	gLoader.ExecElf(sizeof(argv) / sizeof(argv[0]), argv);

	return 0;
}
