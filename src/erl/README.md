## liberl

Liberl is my own ERL loader library, written in C++20.

I decided to write my own ERL loader library since:

 - I need a customizable allocation solution, and the PS2SDK library doesn't support anything like that (even using libc symbols like MAD.)
 - (Opinion!!!) The original code isn't really all too good.
 - I'd like a more minimal library, as I'm setting a 64k max for the main ElfLdr elf.
 - The original library is *VERY* chatty, and I'd only really like that specifically for debugging.

## Usage

This is currently a elfldr internal library. If anyone *wants* me to split it out, I can probably do that.

But that'd involve making the library a bit more compliant to the PS2SDK one.