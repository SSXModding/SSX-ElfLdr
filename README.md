## SSX-Elfldr

A little patching elf loader for SSX OG and SSX3, giving the ability to run the game under HostFS, and a general patching framework for developers.

For OG, it also will offer ERL code loading support, allowing developers to run custom code *while* the game is playing. Pretty cool, huh?

## HostFS Patch

This patch allows the game to load it's assets from the host filesystem, making it easier for both modders to tinker with the game

(now remastering isn't needed) and players to install mods (it also solves the problem of legality, as mods no longer need to be a delta, just the mod's files).

## MemClr patch

This patch disables the memory clear pattern the SSX games do, which is a byproduct of some debugging code left in from the REAL (REwritten EA Library?) library's custom memory allocator.

Really only useful for keeping elfldr around for as long as possible.

## Building

You need PS2SDK installed (the PS2DEV/open source one, not the Sony one).

Change the HostFS path in src/main.cpp to fit your configuration, if required, then run `make`.

To build the SSX3 version of the loader, run `make SSX3=1`.

To build the version with experimental patches run `make EXPERIMENTAL=1`