// Little tiny elf loader thing

// TODO: Trim this down?

#include <iopcontrol.h>
#include <iopheap.h>
#include <kernel.h>
#include <sbv_patches.h>
#include <sifrpc.h>
#include <loadfile.h>
#include <stdio.h>
#include <unistd.h>
#include <input.h>
#include <time.h>
#include <string.h>
#include <libcdvd.h>

// Global exec data, written by SifLoadElf(...)
// We use this to run the elf in ExecElf
t_ExecData gExecData{};

template<class... T>
constexpr void DebugOut(const char* fmt, T... t) {
	char buf[256]{};
 
	// this is PAIN.
	memcpy(&buf[0], "[ElfLdr] ", sizeof("[ElfLdr]"));
	snprintf(&buf[sizeof("[ElfLdr]")], (sizeof(buf) - sizeof("[ElfLdr]") - 1), fmt, t...);
	puts(buf);
}

// reset the IOP, unloading any modules we might have put onto it.
void ResetIOP() {
	SifInitRpc(0);
	
	while(!SifIopReset("", 0));
	while(!SifIopSync());
	
	SifInitRpc(0);
}

void LoadElf(const char* fileName) {
	DebugOut("Trying to load ELF File \"%s\"...", fileName);
	SifLoadElf(fileName, &gExecData);
}

// fun code utilities

inline void replace_string(void* addr, const char* string) {
	DebugOut("Replacing string \"%s\" at %p with \"%s\"...", reinterpret_cast<char*>(addr), addr, string);
	memcpy(addr, string, strlen(string) + 1);
}

// fill an area with mips NOP
// N is the number of instructions to write.
template<size_t N>
constexpr void NopFill(void* start) {
	memset(start, 0x0, N * sizeof(uintptr_t));
}

// no real point for this, it's just nicer to look at,
// and easier to type
template<class T>
constexpr T& RefAt(void* addr) {
	return *static_cast<T*>(addr);
}

void PatchStuff() {
		DebugOut("Doing patching...");
		
		// NOP fill the direct memory clearing loop from bxPreInit()
		NopFill<10>(reinterpret_cast<void*>(0x0018a6d8));
		
		// NOP fill the memory clearing logic from CMN initheapdebug(),
		// as removing the call breaks everything.
		NopFill<32>(reinterpret_cast<void*>(0x0018a294));
		
		// HOSTFS PATCH ATTEMPT (does NOT work yet...)
		// I think I can get this to work though. It'd be pretty awesome at least
		
		//replace_string((void*)0x002c4e70, "host:");
		
		// This is cheeky as all hell.
		// (replaces the strncmp length param constant from 6 to 4, so just "host" needs to be matched)
		//RefAt<uint8_t>((void*)0x00238550) = 0x4;
		// old style C version here just in case:
		//*((uint8_t*)(void*)0x00238550) = 0x4;
		
		DebugOut("Done with patching...");
}

void ExecElf() {
	char* argv[1];

	argv[0] = "cdrom0:";
	
	if(gExecData.epc > 0) {
		// Well, we have a valid elf file, so..

		// flush cache garbage
		FlushCache(0);
		FlushCache(2);
		
		PatchStuff();
		
		// flush cache again just in case
		FlushCache(0);
		FlushCache(2);
		
		DebugOut("Rebooting IOP, after this ExecPS2 time...");
		
		ResetIOP();
		ExecPS2(reinterpret_cast<void*>(gExecData.epc), reinterpret_cast<void*>(gExecData.gp), sizeof(argv)/sizeof(argv[0]), argv);
		return;
	}
	
	DebugOut("Ouch, LoadElf() failed. Hanging.");
	while(1);
}

int main() {
	DebugOut("Hello world?");
	
	// Do some initalization, and load some IOP modules.
	ResetIOP();
	SifInitIopHeap();
	SifLoadFileInit();
	
	// ?
	//sbv_patch_disable_prefix_check();
	
	// Load some ROM modules
	SifLoadModule("rom0:SIO2MAN", 0, NULL);
	SifLoadModule("rom0:MCMAN", 0, NULL);
	SifLoadModule("rom0:MCSERV", 0, NULL);
	SifLoadModule("rom0:PADMAN", 0, NULL);
	sceCdInit(SCECdINoD);
	
	LoadElf("cdrom0:\\SLUS_200.95");
	ExecElf();
	
	return 0;
}