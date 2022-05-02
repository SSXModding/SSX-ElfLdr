# Setup Guide

**NOTE:** A graphical setup program which automatically handles ISO/physical disc extraction will be provided soon, however this manual setup guide will still be updated.

For now:

- Follow the game specific instructions first (they are below this initial step).
- Copy `bin/elfldr.elf` from the release .zip file (or `elfldr.elf` in your build, if compiling from source) alongside the main game ELF in the directory you chose.
- In PCSX2:
    - Enable HostFS by checking "System->Game Settings->Enable Host Filesystem".
    - File->Run ELF the `elfldr.elf` file for the game you extracted.
    - Enjoy your ElfLdr-enhanced game!

# Game Specific Setup

## SSX OG

- Extract the game's files from the ISO or physical disk into a directory of your choosing.
- Using a .BIG extraction tool of your choice, extract the .BIG archives in `data/models`.
    - Once done, copy the files from the newly-made `data/models` to the actual `data/models` directory.

## SSX Tricky

- Extract the game's files from the ISO or physical disk into a directory of your choosing.
- Using a .BIG extraction tool of your choice, extract the .BIG archives in `data/models`.
    - Once done, copy the files from the newly-made `data/models` to the actual `data/models` directory.

## SSX 3

- Extract the game's files from the ISO or physical disk into a directory of your choosing.
- (Up to you) Remove the two .PAD files, they are not needed by the game.