# Building SSX-ElfLdr From Source

## Dependencies

You need PS2SDK installed (the PS2DEV/open source one, not the Sony one).

It can be gathered [here](https://github.com/ps2dev/ps2toolchain) if you do not have it installed.

### Optional Dependencies

If you are going to build the documentation, you will also need `mkdocs`.

Using pip, it can be installed like `pip install mkdocs`.

## Build

```bash
$ cmake -B build -GNinja -DCMAKE_TOOLCHAIN_FILE=cmake/toolchain/ps2.cmake
$ cmake --build build
```

## Building Packages

It is fairly easy to build a ZIP package exactly like the ones that are posted on GitHub Releases.

Assuming you have followed [Build](#build) and installed the dependencies outlined in [Optional Dependencies](#optional-dependencies):

```bash
$ cmake --build build -t package
```

Check build/packages for the generated ZIP file.

## Building this documentation

If CMake detected your MkDocs installation, it should have printed something like:

```text
-- Enabling documentation build
```

This means that any build command, like the ones seen above, will automatically build the documentation.

### Manually building

You can use either the `docs` target in CMake, or run `mkdocs build` directly in the root of the project.