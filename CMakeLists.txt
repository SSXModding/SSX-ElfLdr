cmake_minimum_required(VERSION 3.19)

# Prohibit in-source tree builds.
# CMake will be banning this in a bit, so when we can adjust cmake_minimum_required
# to that exact CMake version, this block can be removed (or the policy enforced).
if(" ${CMAKE_SOURCE_DIR}" STREQUAL " ${CMAKE_BINARY_DIR}")
    message(FATAL_ERROR "In-source-tree builds are strictly prohibited. Please don't do them.")
endif()

include(${CMAKE_SOURCE_DIR}/cmake/Policies.cmake)
project(elfldr)

# require C++20. we are gamikoewasdgnfkjlljdeagfnkjjs.
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Git tag
add_custom_target(__elfldr_gittag
        COMMAND ${CMAKE_COMMAND} -P ${PROJECT_SOURCE_DIR}/cmake/GitTag.cmake
        WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
        SOURCES ${PROJECT_BINARY_DIR}/Version.h)
set_source_files_properties(${PROJECT_BINARY_DIR}/Version.h PROPERTIES GENERATED TRUE)

# Include elfldr modules
add_subdirectory(src/utils)
add_subdirectory(src/erl)

# elfldr itself :)
add_subdirectory(src/elfldr)