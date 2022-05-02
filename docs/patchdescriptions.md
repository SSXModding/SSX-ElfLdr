# Patch Descriptions

### HostFS Patch

This patch allows the game to load files entirely from a folder on your computer.

This makes it easier for modders to tinker with the game.

It also makes it easier for players to install mods.

It finally also solves legality issues as mods no longer need to be distributed as the full game.

### MemClr patch

This patch disables the memory clear pattern the SSX games do, which is a byproduct of some debugging code left in from
the REAL (REwritten EA Library?) library's custom memory allocator.

### Experimental patch

This patch is mostly a dumping ground for very untested code, and as of right now serves as an ERL loader.

It also registers the heap allocator used by elfldr (for now).

