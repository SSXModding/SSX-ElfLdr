## SSX-Elfldr

A patching loader for the SSX games on PS2, providing the ability to run the game from a folder on the host computer.

It also provides a general patching/function hooking framework for developers.

## Builds
Automatic builds of Elfldr, made on every commit to every branch, are available [at the Releases tab](https://github.com/modeco80/SSX-ElfLdr/releases).

Versioned releases will also be available here, but you can choose to use the bleeding edge (if you so desire).

## Game Support Table
Game regions/versions or whole games marked as Unsupported are planned to be supported later.



| Game          | Region/Version     | Support Status
| --------------|--------------------|---------------|
| SSX OG (2000) | USA (1.0)          | ✅
| SSX OG (2000) | USA (Greatest Hits)| ❓
| SSX OG (2000) | PAL (1.0)          | ❌
| SSX OG (2000) | NTSC-J (1.0)       | ❌
| SSX Tricky    | USA (1.0)          | ✅
| SSX Tricky    | PAL/NTSC-J         | ❌
| SSX 3         | OPSM2 Demo         | 🧩️
| SSX 3         | KR Demo            | 🧩
| SSX 3         | PAL/NTSC-J         | ❌
| SSX On Tour   | (N/A)              | ❌


#### Legend
| Symbol   |  Meaning
| ---------|-----------------------|
|  ✅️      | Supported
|  🧩️       | Partial/In development
|  ❌️      | Unsupported
|  ❓       | Unknown


## Setup
**NOTE:** A graphical setup program which automatically handles ISO/physical disc extraction will be provided soon. 

For now:

- Follow the game specific instructions first (they are below this initial step).
- Copy `elfldr.elf` alongside the main game ELF in the directory you chose.
- In PCSX2:
    - Enable HostFS by checking "System->Game Settings->Enable Host Filesystem".
    - "Run ELF" the `elfldr.elf` file for the game you extracted.
    - Enjoy your Elfldr-enhanced game!

### SSX OG
- Extract the game's files from the ISO or physical disk into a directory of your choosing.
- Using a .BIG extraction tool of your choice, extract the .BIG archives in `data/models`.
  - Once done, copy the files from the newly-made `data/models` to the actual `data/models` directory.

### SSX Tricky
- Extract the game's files from the ISO or physical disk into a directory of your choosing.

### SSX 3
- Extract the game's files from the ISO or physical disk into a directory of your choosing.
- (Up to you) Remove the two .PAD files, they are not needed by the game.

## Patch Descriptions

### HostFS Patch

This patch allows the game to load entirely from a folder on your computer.

This makes it easier for modders to tinker with the game.

It also makes it easier for players to install mods.

It finally also solves legality issues as mods no longer need to be distributed as the full game.

### MemClr patch

This patch disables the memory clear pattern the SSX games do, which is a byproduct of some debugging code left in from the REAL (REwritten EA Library?) library's custom memory allocator.

### Experimental patch

This patch is mostly a dumping ground for very untested code, and as of right now serves as an ERL loader.

It also registers the memory allocator used by elfldr (for now).

## Building from source

You need PS2SDK installed (the PS2DEV/open source one, not the Sony one).

It can be gathered [here](https://github.com/ps2dev/ps2toolchain) if you do not have it installed.

To build:

```bash
$ cmake -B build -GNinja -DCMAKE_TOOLCHAIN_FILE=cmake/toolchain/ps2.cmake
$ cd build
$ cmake --build .
# Profit? (elfldr.elf in build/)
```
