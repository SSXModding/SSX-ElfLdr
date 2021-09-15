// Little tiny elf loader thing

// TODO: Trim this down?

#include <iopcontrol.h>
#include <iopheap.h>
#include <kernel.h>
#include <sifrpc.h>
#include <loadfile.h>
#include <libcdvd.h>

#include "codeutils.h"

// Global exec data, written by SifLoadElf(...)
// We use this to run the elf in ExecElf
t_ExecData gExecData{};

// reset the IOP, unloading any modules we might have put onto it.
void ResetIOP() {
	SifInitRpc(0);
	
	while(!SifIopReset("", 0));
	while(!SifIopSync());
	
	SifInitRpc(0);
}

void LoadElf(const char* fileName) {
	util::DebugOut("Trying to load ELF File \"%s\"...", fileName);
	SifLoadElf(fileName, &gExecData);
}

void PatchStuff() {
	util::DebugOut("Doing patching...");
	
	// These addresses are ALL for the US release.
	// Sorry, it's just what I have in Ghidra.
	
	// NOP fill the direct memory clearing loop in bxPreInit()
	util::NopFill<10>(reinterpret_cast<void*>(0x0018a6d8));
	
	// NOP fill the memory clearing logic in CMN initheapdebug(),
	// as simply NOP filling the call causes the game to crash.
	util::NopFill<32>(reinterpret_cast<void*>(0x0018a294));
	
	// HostFS patch:
	
	// ASYNCFILE_init usually gets "cd:".
	// We replace this with a string which will match "host"
	util::ReplaceString(reinterpret_cast<void*>(0x002c4e70), "host");
	
	// Replace the strncmp length param constant in ASYNCFILE_init 
	// (by altering the instruction. Worst hack ever.)
	// from 6 to 4, so we can just use "host:".
	// This could be -= 2, but meh. This is more direct and better.
	util::MemRefTo<uint8_t>(reinterpret_cast<void*>(0x00238550)) = 0x4;
	
	// Switch path normalization when making the "beautiful" path
	// around so it uses the Windows path seperation character
	// on linux you may actually need to remove this
	//
	// this actually isn't needed. I'm keeping it here in case it is:
	//
	//util::MemRefTo<uint8_t>(reinterpret_cast<void*>(0x0023770c)) = '/';
	//util::MemRefTo<uint8_t>(reinterpret_cast<void*>(0x00237710)) = '\\';
	
	// where our host path string should be placed
	constexpr static uintptr_t STRING_ADDRESS = 0x002c5cc0;
	
	// write a new string in some slack space, and then make
	// the pointer the game uses our slack space pointer.
	// IDK why the string isn't exactly written at 2c5cc0, so we go 4 forwards. It works.
	// I'm not questioning it
	util::MemRefTo<uintptr_t>(reinterpret_cast<void*>(0x002c59c8)) = STRING_ADDRESS + 4;
	util::WriteString(reinterpret_cast<void*>(STRING_ADDRESS), "host:C:\\pcsx2\\bin\\ssxmod\\");
	
	util::DebugOut("Done with patching...");
}

void ExecElf() {
	char* argv[1];

	argv[0] = "host:C:\\pcsx2\\bin\\ssxmod\\";
	
	if(gExecData.epc > 0) {
		PatchStuff();
		ResetIOP();
		ExecPS2(reinterpret_cast<void*>(gExecData.epc), reinterpret_cast<void*>(gExecData.gp), sizeof(argv)/sizeof(argv[0]), argv);
		return;
	}
	
	util::DebugOut("LoadElf() failed. Hanging.");
	while(1);
}

int main() {
	util::DebugOut("Hello world?");
	
	// Do some initalization, and load some IOP modules.
	ResetIOP();
	SifInitIopHeap();
	SifLoadFileInit();
	
	// Load some ROM modules
	SifLoadModule("rom0:SIO2MAN", 0, NULL);
	SifLoadModule("rom0:MCMAN", 0, NULL);
	SifLoadModule("rom0:MCSERV", 0, NULL);
	SifLoadModule("rom0:PADMAN", 0, NULL);
	sceCdInit(SCECdINoD);
	
	// load n' exec
	LoadElf("host:C:\\pcsx2\\bin\\ssxmod\\SLUS_200.95");
	ExecElf();
	
	return 0;
}