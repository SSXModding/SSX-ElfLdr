# LibUtils

Utils is the ModLoader utility library; used for a lot of stuff which can be shared.

It has:

 - `DebugOut()`: debug printing
 - `FioFile` & `FioDirectory`: wrapper classes for safe access to raw FIO APIs
 - `GameVersion` : Type for describing a game version elegantly
   - `AutodetectGameVersion()`: automatically detect and fill out the global `GameVersion`.
   - `SetupAllocator()`: Setup the [mlstd](mlstd.md) allocator from the global `GameVersion` automatically.
 - `HookFunction<HookT>()` : Function hooking (& trampolining).
 - MIPS instruction encoder routines
 - Other general code utilities. 
