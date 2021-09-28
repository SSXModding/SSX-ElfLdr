## SSX-Elfldr

A little patching elf loader for SSX OG and SSX3, giving the ability to run the game under HostFS.

For OG, it also will offer ERL code loading support.

## HostFS Patch

This patch allows the game to load it's assets from the host filesystem, making it easier

## MemClr patch

This

## Building

You need PS2SDK installed (the PS2DEV one, not sce).

Change the HostFS path in src/main.cpp to fit your configuration, if required, then run `make`.

To build the SSX3 version of the loader, run `make SSX3=1`.

To build the version with experimental patches run `make EXPERIMENTAL=1`
