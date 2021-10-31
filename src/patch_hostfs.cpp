// HostFS patch - allows game files to be loose
// on the filesystem, so it's easier to tinker with them.
//
// How this patch works is pretty simple.
//
//	- It first tricks the game into going into the mode where it would use "host0:",
//		by replacing the string it uses with just "host", from "cd:".
//		It also alters how many characters strncmp() will look at,
//		to make it so we don't need to supply "host0:", just "host".
//
//	- Once that's done, we write the HostFS path in a little bit of slack space,
//		and alter a pointer which usually points to a string containing "host0:"
//		to point to our slack space HostFS path string.
//
//
// Note that (including path seperators) there is about a 31 character
// limit for the HostFS path. This techinically could be fixable by making
// the game use a slack space rather than a stack array, however, that would be
// a lot more painful, and would be a far more complicated patch.
// A minor tradeoff to make this patch so much easier is okay, at least to me..

#include "patch.h"

#include <utils.h>
#include <codeutils.h>

#include "GameVersion.h"

// in main.cpp
extern const char* gHostFsPath;

using namespace elfldr;

struct HostFsPatch : public Patch {
	const char* GetName() const override {
		return "HostFS";
	}

	const char* GetIdentifier() const override {
		return "hostfs";
	}

	bool IsCompatiable() const override {
		const auto& data = GetGameVersionData();

		if(data.region != GameRegion::NTSC)
			return false;

		if(data.game == Game::SSXOG || data.game == Game::SSXDVD)
			return true;

		return false;
	}

	void Apply() override {
		const auto& data = GetGameVersionData();

		switch(data.game) {
			case Game::SSXOG: {
				// ASYNCFILE_init usually gets "cd:".
				// We replace this with a string which will match "host",
				// after we..
				util::ReplaceString(util::Ptr(0x002c4e70), "host");

				// replace the strncmp length param constant in ASYNCFILE_init
				// from 6 to 4, so we can just use "host".
				util::MemRefTo<std::uint8_t>(util::Ptr(0x00238550)) = 0x4;

				// Write a new string in some slack space.

				util::WriteString(util::Ptr(0x002c5cc4), gHostFsPath);

				// Overwrite the pointer that the path "beautification" function uses to strcat()
				// "host0:" pointing it to our HostFS path instead.
				util::MemRefTo<std::uint32_t>(util::Ptr(0x002c59c8)) = 0x002c5cc4;

				// Write new IOP module paths
				util::WriteString(util::Ptr(0x002b3ab0), "host:data/modules/ioprp16.img");
				util::WriteString(util::Ptr(0x002b3b08), "host:data/modules/sio2man.irx");
				util::WriteString(util::Ptr(0x002b3b48), "host:data/modules/padman.irx");
				util::WriteString(util::Ptr(0x002b3b88), "host:data/modules/libsd.irx");
				util::WriteString(util::Ptr(0x002b3bc8), "host:data/modules/sdrdrv.irx");
				util::WriteString(util::Ptr(0x002b3c08), "host:data/modules/snddrv.irx"); // eac custom!!!
				util::WriteString(util::Ptr(0x002b3c48), "host:data/modules/mcman.irx");
				util::WriteString(util::Ptr(0x002b3c88), "host:data/modules/mcserv.irx");

				// This will completely disable loading worlds from BIG files.
				// Only enable this if you've extracted everything!!!
#if 1
				util::NopFill<3>(util::Ptr(0x00187704)); // nop TheApp.MountWorld(...) in cGame::cGame()
				util::NopFill<2>(util::Ptr(0x001879f4)); // nop TheApp.UnmountWorld() in cGame::~cGame()
#endif

				// you know what? fuck you
				// *unbigs your files*
				// (i could patch bxMain() but cApplication::Run() never returns in release.)
				//util::NopFill<36>(util::Ptr(0x00183b68));

				// Rewrite most of the cWorld path strings to remove the |.
				// This allows world files to either be loose or inside of the venue BIG files
				// (as long as the above code is not enabled).

				// I don't think this is ever used cause the game mounts the big
				// before calling cWorld::Load(). Maybe older versions of the function
				// mounted the BIG file from this path itself? We may never know
				// (unless said older builds leak of course..)
				util::WriteString(util::Ptr(0x002bdfc0), "data/models/%s.big");

				// Actually used paths.
				util::WriteString(util::Ptr(0x002bdfd8), "data/models/%s.wdx");
				util::WriteString(util::Ptr(0x002bdff0), "data/models/%s.wdf");
				util::WriteString(util::Ptr(0x002be008), "data/models/%s.wdr");
				util::WriteString(util::Ptr(0x002be020), "data/models/%s.wdv");
				util::WriteString(util::Ptr(0x002be038), "data/models/%s.wds");
				util::WriteString(util::Ptr(0x002be050), "data/models/%s.wfx");
				util::WriteString(util::Ptr(0x002be068), "data/models/%s.aip");
				util::WriteString(util::Ptr(0x002be080), "data/models/%s.ssh");
				util::WriteString(util::Ptr(0x002be098), "data/models/%sl.ssh");
				util::WriteString(util::Ptr(0x002b6d10), "data/models/%s_sky");
			} break;

				// From now on I'm gonna comment a bit less cause the above explains
				// the rest pretty well. If you neeed help, pretty much just reference
				// the above comments (with some per-game changes).

			case Game::SSXDVD: {
				util::ReplaceString(util::Ptr(0x00387468), "host0:");
				util::WriteString(util::Ptr(0x003b9130), "host:");

				// Write new IOP module paths
				util::WriteString(util::Ptr(0x00387258), "host:data/modules/ioprp224.img");
				util::WriteString(util::Ptr(0x003872b0), "host:data/modules/sio2man.irx");
				util::WriteString(util::Ptr(0x003872f0), "host:data/modules/padman.irx");
				util::WriteString(util::Ptr(0x00387330), "host:data/modules/libsd.irx");
				util::WriteString(util::Ptr(0x00387370), "host:data/modules/snddrv.irx");
				util::WriteString(util::Ptr(0x003873b0), "host:data/modules/mcman.irx");
				util::WriteString(util::Ptr(0x003873f0), "host:data/modules/mcserv.irx");

				// TODO: Bigless
			} break;
		}
	}
};

// Register the patch into the patch system
static PatchRegistrar<HostFsPatch, 0x01> registrar;