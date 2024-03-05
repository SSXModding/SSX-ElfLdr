#
# SSX-Elfldr
#
# (C) 2021-2022 Lily/modeco80 <lily.modeco80@protonmail.ch>
# under the terms of the MIT license.
#

add_executable(elfldr
        # Elfldr core
        ElfLoader.cpp
        ElfPatch.cpp

        # patches
        Patches/HostFS.cpp
        Patches/MemoryClear.cpp

        # entry point
        main.cpp
        )

target_include_directories(elfldr PRIVATE ${PROJECT_BINARY_DIR} ${CMAKE_CURRENTS_SOURCE_DIR})
add_dependencies(elfldr __elfldr_gittag)

# -Wl,--start-group -lc_nano -lkernel -lps2sdkc -Wl,--end-group

# This ugliness is so we don't link anything we don't need.
# We also prefer/use nano Newlib, to make the executable even smaller.
target_link_libraries(elfldr
        "-nodefaultlibs"
        "-Wl,--start-group"
        gcc
        
        # System libraries
        c_nano
        kernel
	cglue

        # Project libraries
        elfldr::utils_elf
        elfldr::mlstd

	
        # add for v0.6.0
        #elfldr::erl
        "-Wl,--end-group")

# Make elfldr output in the root of the build directory, mostly for ease.
set_target_properties(elfldr PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
        )

# Run the user's publish script when elfldr builds
