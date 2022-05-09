# mlstd

The ModLoader Standard Library.

# Goal/features

- Allow efficient C++20 programming
- Not held back by standardese, so any improvements can be checked in easily
- Usable in a freestanding (only needs a few LibC functions and working memory allocation for full usage) environment
- No unnecessary features (e.g: IoStreams) for ModLoader
  - See [LibUtils](utils.md) for that 

# When should code be checked into `mlstd` instead of a project tree?

If it:

 - is an algorithm working with a mlstd type which is general
 - is a container type which can be used by multiple clients
 - in general is sharable between ModLoader components
 
then it should *probably* be put in mlstd. 

Exceptions to this rule are:

 - Anything dealing with game versioning (go to Utils, collect 200$)
 - Specific to a library (if it's a hack for instance, leave it in that project or fix the issue properly)
