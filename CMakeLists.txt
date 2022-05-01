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

include(${CMAKE_SOURCE_DIR}/cmake/Policies.cmake)
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

# Include elfldr modules
add_subdirectory(src/runtime)
add_subdirectory(src/utils)
add_subdirectory(src/erl)

# elfldr itself :)
add_subdirectory(src/elfldr)

add_subdirectory(src/sampleerl)


## project packaging

set(CMAKE_INSTALL_PREFIX /)
set(CPACK_SET_DESTDIR ON)

install(TARGETS elfldr)

# documentation
install(FILES ${PROJECT_SOURCE_DIR}/README.md ${PROJECT_SOURCE_DIR}/LICENSE DESTINATION ${CMAKE_INSTALL_PREFIX})
# todo: mkdocs generated page in /doc

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
