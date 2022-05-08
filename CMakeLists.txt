#
# SSX-Elfldr
#
# (C) 2021-2022 Lily/modeco80 <lily.modeco80@protonmail.ch>
# under the terms of the MIT license.
#

cmake_minimum_required(VERSION 3.19)

##if(NOT CMAKE_SYSTEM_NAME STREQUAL "Playstation2")
##    add_subdirectory(cmake/superbuild)
##    return()
##endif()


# Prohibit in-source tree builds.
# CMake will be banning this in a bit, so when we can adjust cmake_minimum_required
# to that exact CMake version, this block can be removed (or the policy enforced).
if(" ${CMAKE_SOURCE_DIR}" STREQUAL " ${CMAKE_BINARY_DIR}")
    message(FATAL_ERROR "In-source-tree builds are strictly prohibited. Please don't do them.")
endif()

include(cmake/CMakeNewPolicy.cmake)

project(elfldr
        LANGUAGES CXX
        DESCRIPTION "HostFS enabling Mod/ELF loader for the SSX games on PS2"
        VERSION 0.5
        )

# Require C++20. It's 2022,
# we're 80% of our way to C++23.
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)


# Git tag target.
add_custom_target(__elfldr_gittag
        COMMAND ${CMAKE_COMMAND} -P ${PROJECT_SOURCE_DIR}/cmake/GitTag.cmake
        WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
        SOURCES ${PROJECT_BINARY_DIR}/Version.h)
set_source_files_properties(${PROJECT_BINARY_DIR}/Version.h PROPERTIES GENERATED TRUE)

# ElfLdr mkdocs
include(MkDocs)

# Include elfldr modules
add_subdirectory(src/mlstd)
add_subdirectory(src/utils)
add_subdirectory(src/erl)

# elfldr itself :)
add_subdirectory(src/elfldr)

add_subdirectory(src/sampleerl)


## project packaging

set(CMAKE_INSTALL_PREFIX /)
set(CPACK_SET_DESTDIR ON)

# documentation
install(FILES ${PROJECT_SOURCE_DIR}/README.md ${PROJECT_SOURCE_DIR}/LICENSE DESTINATION /)

# Install static mkdocs documentation
# if documentation build is enabled
if(DOCS_ENABLED)
    install(DIRECTORY ${PROJECT_SOURCE_DIR}/site/ DESTINATION /doc)
endif()

# todo: mkdocs generated page in /doc

install(TARGETS elfldr)

# SDK stuffs.
install(TARGETS mlstd elfldr_utils_erl DESTINATION /sdk/lib)
install(DIRECTORY ${PROJECT_SOURCE_DIR}/include/ DESTINATION /sdk/include)
install(DIRECTORY ${PROJECT_SOURCE_DIR}/cmake/Toolchain/ DESTINATION /sdk/toolchain)
install(DIRECTORY ${PROJECT_SOURCE_DIR}/src/sampleerl/ DESTINATION /sdk/samples/sample_ch)

## CPack (used by GitHub Actions to package releases)

if(NOT DEFINED CPACK_GENERATOR)
        set(CPACK_GENERATOR ZIP)
endif()

set(CPACK_VERBATIM_VARIABLES        ON)
set(CPACK_ARCHIVE_THREADS           0)
set(CPACK_PACKAGE_DIRECTORY         ${PROJECT_BINARY_DIR}/packages)
set(CPACK_PACKAGE_NAME              SSX-ElfLdr)
set(CPACK_PACKAGE_VENDOR            modeco80)
set(CPACK_PACKAGE_CONTACT           modeco80)
set(CPACK_RESOURCE_FILE_README      ${PROJECT_SOURCE_DIR}/README.md)
set(CPACK_RESOURCE_FILE_LICENSE     ${PROJECT_SOURCE_DIR}/LICENSE)
set(CPACK_PACKAGE_INSTALL_DIRECTORY elfldr)


include(CPack)
