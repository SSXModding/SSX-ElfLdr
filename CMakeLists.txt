cmake_minimum_required(VERSION 3.19)

# Prohibit in-source tree builds.
# CMake will be banning this in a bit, so when we can adjust cmake_minimum_required
# to that exact CMake version, this block can be removed (or the policy enforced).
if(" ${CMAKE_SOURCE_DIR}" STREQUAL " ${CMAKE_BINARY_DIR}")
    message(FATAL_ERROR "In-source-tree builds are strictly prohibited. Please don't do them.")
endif()

include(${CMAKE_SOURCE_DIR}/cmake/Policies.cmake)
project(elfldr)

# Require C++20. It's 2022,
# we're 80% of our way to C++23.
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Verify developer is using the PS2 toolchain,
# and not e.g: the host toolchain.
if(NOT CMAKE_SYSTEM_NAME STREQUAL "Playstation2")
    # TODO(setuptool/superbuild): This shouldn't be an error, we should just do the host build
    message(FATAL_ERROR "Something is totally wrong with your environment. Did you not do -DCMAKE_TOOLCHAIN_FILE=/path/to/elfldr/cmake/Toolchain/ps2.cmake ?")
endif()

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