#
# SSX-Elfldr
#
# (C) 2021-2022 Lily/modeco80 <lily.modeco80@protonmail.ch>
# under the terms of the MIT license.
#

add_library(mlstd

        # C runtime replacement code
        # crt/
        crt/ps2sdk_stubs.cpp
        crt/printf.cpp

        # C++ Runtime code
        # ./
        Allocator.cpp
        Error.cpp
        String.cpp
        Fnv1a.cpp
        )

target_include_directories(mlstd PUBLIC ${PROJECT_SOURCE_DIR}/include/)

# use these aliases thx :)
add_library(elfldr::mlstd ALIAS mlstd)