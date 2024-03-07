/**
 * SSX-Elfldr
 *
 * (C) 2021-2022 Lily/modeco80 <lily.modeco80@protonmail.ch>
 * under the terms of the MIT license.
 */

#include <mlstd/Assert.h>
#include <utils/Utils.h>

#include "ElfLoader.h"
#include "ElfPatch.h"
#include "mlstd/Bit.h"

// Autogenerated version header
#include <mlstd/DynamicArray.h>
#include <stdio.h>
#include <utils/GameVersion.h>
#include <utils/VersionProbe.h>
#include <Version.h>

elfldr::ElfLoader gLoader;

int main() {
	elfldr::util::DebugInit();

#ifndef NDEBUG
	elfldr::util::DebugOut("SSX-ElfLdr version " ELFLDR_VERSION_TAG " (" __DATE__ " " __TIME__ ")");
#else
	elfldr::util::DebugOut("SSX-ElfLdr version " ELFLDR_VERSION_TAG);
#endif

	// Probe the game version.
	// TODO: If config file found, parse it, load in the game version, and verify that
	// 		that binary exists. If it doesn't exist, bail back to OSD.
	elfldr::util::AutodetectGameVersion();

	const auto& gdata = elfldr::util::GetGameVersionData();

	if(gdata.game == elfldr::util::Game::Invalid) {
		// TODO: if config file, change this message to state "Invalid game configured in config file {config}".
		elfldr::util::DebugOut("No supported game could be detected alongside ModLoader.");
		elfldr::util::DebugOut("Make sure the ModLoader is in the proper spot.");
		while(true)
			;
	}

	// TODO: only display if auto-detecting the game
	elfldr::util::DebugOut("GameID: \"%s\"", gdata.GameID().CStr());

	// Load the ELF int memory. This won't clobber us because we load very high in memory,
	// at least compared to the normal PS2 linker scripts
	{
		static char elfPath[elfldr::util::MaxPath] {};
		snprintf(&elfPath[0], elfldr::util::MaxPath * sizeof(char), "host:%s", gdata.GetGameBinary().CStr());
		MLSTD_VERIFY(gLoader.LoadElf(elfPath));
	}

	// Set up the mlstd memory allocator automagically.
	//
	// Once this is called we can use pretty much all the fun things
	// the C++ environment has, and have effectively left
	// the "bootstrap" state, and can now actually apply patches and stuff..
	//
	// TODO: Recognize that this is an operation that can fail and abort if it does.
	elfldr::util::SetupAllocator();

	auto ApplyElfPatch = [](elfldr::ElfPatch* patch) {
		if(patch == nullptr)
			return;

		elfldr::util::DebugOut("[Patch %s] Applying patch...", patch->GetName());
		patch->Apply();
		elfldr::util::DebugOut("[Patch %s] Finished applying.", patch->GetName());
	};

	// Apply the basic ELF patches, HostFS and MemoryClear.
	ApplyElfPatch(elfldr::GetPatchById(0x00));
	ApplyElfPatch(elfldr::GetPatchById(0x01));

	// test dynamicarray

	mlstd::DynamicArray<int> ints;

	for(uint32_t i = 0; i < 24; ++i) {
		if((i & 1))
			ints.PushBack(0x41414141);
		else
			ints.PushBack(0x42424242);
	}

	// print the address of the array so we can look at it
	elfldr::util::DebugOut("ints.Data() = %p", ints.Data());

#if 0
	for(uint32_t i = 0; i < ints.Size(); ++i) {
		elfldr::util::DebugOut("ints[%u] = 0x%08x", i, ints[i]);
	}
#endif

	// test rangefor
	for(auto& num : ints) {
		elfldr::util::DebugOut("num is 0x%08x", num);
	}

	// test that string SSO's when it should and heap allocates when it should
	mlstd::String stackStr = "HI STACK";
	mlstd::String heapStr = "Hello World this string should be allocated on the heap";
	elfldr::util::DebugOut("stackStr: %s/%p\n", stackStr.data(), stackStr.data());
	elfldr::util::DebugOut("heapStr: %s/%p\n", heapStr.data(), heapStr.data());

	// Load codehooks into memory and initialize them.

	// Execute the game ELF.
	char* argv[3];
	argv[0] = mlstd::BitCast<char*>("host:"); // I hate this
	argv[1] = mlstd::BitCast<char*>("-track");
	argv[2] = mlstd::BitCast<char*>("host:data/worlds/bam.big");

	elfldr::util::DebugOut("Executing game ELF (end of resident execution)\n");

	elfldr::util::DebugClose();
	gLoader.ExecElf(sizeof(argv) / sizeof(char**), argv);

	return 0;
}
