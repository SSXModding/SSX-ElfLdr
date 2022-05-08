#
# SSX-Elfldr
#
# (C) 2021-2022 Lily/modeco80 <lily.modeco80@protonmail.ch>
# under the terms of the MIT license.
#

# These sources are in both libutils-elf
# and libutils-erl (the minimized version for the ERL to use.)
set(__ELFLDR_UTILS_BASE_SOURCES
        assert.cpp
        CodeUtils.cpp
        debugout.cpp
        Hook.cpp
        AllocatorSetup.cpp
        GameVersion.cpp

        # SDK things:
        GameApi.cpp
        )

add_library(elfldr_utils_elf
        ${__ELFLDR_UTILS_BASE_SOURCES}
        FioFile.cpp
        FioDirectory.cpp
        # This depends on FioDirectory, which currently is only provided in the ELF version.
        # I may move it to the ERL version if FioFile/FioDirectory can be moved there.
        VersionProbe.cpp
        )

target_include_directories(elfldr_utils_elf PUBLIC ${PROJECT_SOURCE_DIR}/include/)

# ERL version
add_library(elfldr_utils_erl
        ${__ELFLDR_UTILS_BASE_SOURCES}
        )

target_include_directories(elfldr_utils_erl PUBLIC ${PROJECT_SOURCE_DIR}/include/)


# use these aliases thx :)
add_library(elfldr::utils_elf ALIAS elfldr_utils_elf)
add_library(elfldr::utils_erl ALIAS elfldr_utils_erl)