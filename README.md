## SSX-Elfldr

A little patching elf loader, for SSX OG (more TODO), giving the ability to run the game under HostFS.

## HostFS Patch

TODO: writeup

## MemClr patch

TODO: writeup

## Building

You need PS2SDK installed (the PS2DEV one, not sce).

Change the HostFS path in src/main.cpp to fit your configuration, if required, then run `make`.

To build the SSX3 version of the loader, run `make SSX3=1`.
