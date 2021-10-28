## SSX-Elfldr

A little patching elf loader for SSX OG and SSX3, giving the ability to run the game under HostFS, and a general patching framework for developers.

For OG, it also will offer ERL code loading support, allowing developers to run custom code *while* the game is playing. Pretty cool, huh?

## Building

You need PS2SDK installed (the PS2DEV/open source one, not the Sony one).

To build the typical version, run `make`.

To build the version with experimental patches run `make EXPERIMENTAL=1`

## Patch Descriptions

### HostFS Patch

This patch allows the game to load entirely from the host filesystem, making it easier for both modders to tinker with the game (now that remastering isn't needed) and players to install mods (it also solves the problem of legality).

### MemClr patch

This patch disables the memory clear pattern the SSX games do, which is a byproduct of some debugging code left in from the REAL (REwritten EA Library?) library's custom memory allocator.

