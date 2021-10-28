## SSX-Elfldr

A little patching elf loader for SSX OG, giving the ability to run the game under HostFS, and a general patching framework for developers.

For OG, it also will offer ERL code loading support, allowing developers to run custom code *while* the game is playing. Pretty cool, huh?

## Game Support

Game regions/versions or whole games marked as Unsupported are planned to be supported later.

| Game          | Region/Version     | Support Status
| --------------|--------------------|---------------|
| SSX OG (2000) | USA (1.0)          | Supported
| SSX OG (2000) | USA (Greatest Hits)| Unknown
| SSX OG (2000) | PAL (1.0)          | Unsupported
| SSX OG (2000) | NTSC-J (1.0)       | Unsupported
| SSX Tricky    | (N/A)              | Unsupported
| SSX 3         | (N/A)              | Unsupported
| SSX On Tour   | (N/A)              | Unsupported

## Building

You need PS2SDK installed (the PS2DEV/open source one, not the Sony one).

To build the typical version, run `make`.

To build the version with experimental patches run `make EXPERIMENTAL=1`

## Patch Descriptions

### HostFS Patch

This patch allows the game to load entirely from the host filesystem, making it easier for both modders to tinker with the game (now that remastering isn't needed) and players to install mods (it also solves the problem of legality).

#### Setup (SSX OG)
 - Extract all of the game's files from the ISO into a directory of your choosing.
 - Once you've done so, using a .BIG extraction tool, extract all of the .BIG archives in data/models, and place all of the files made in that folder.
 - Copy `ssx_elfldr.elf` alongside the main game ELF (e.g: for US, `SLUS_200.95`).

### MemClr patch

This patch disables the memory clear pattern the SSX games do, which is a byproduct of some debugging code left in from the REAL (REwritten EA Library?) library's custom memory allocator.

