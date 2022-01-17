# Based off the shipped ps2dev.cmake toolchain file
# in the ps2sdk source tree, modified for elfldr

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_VERSION 1)
set(CMAKE_SYSTEM_PROCESSOR mips)

set(CMAKE_C_COMPILER mips64r5900el-ps2-elf-gcc)
set(CMAKE_CXX_COMPILER mips64r5900el-ps2-elf-g++)

# We define NEWLIB_PORT_AWARE here because
# we don't use libc file IO (by choice, to save space)
set(CMAKE_C_FLAGS_INIT "-I$ENV{PS2SDK}/ee/include -I$ENV{PS2SDK}/common/include -D_EE -DNEWLIB_PORT_AWARE -G0 -fno-stack-protector -fno-ident -fno-unwind-tables -fno-asynchronous-unwind-tables")

set(CMAKE_C_FLAGS_RELEASE_INIT "${CMAKE_C_FLAGS_INIT} -fomit-frame-pointer")

set(CMAKE_CXX_FLAGS_INIT "${CMAKE_C_FLAGS_INIT} -fno-rtti -fno-exceptions -fno-threadsafe-statics")

set(CMAKE_CXX_FLAGS_RELEASE_INIT "${CMAKE_C_FLAGS_RELEASE_INIT} ${CMAKE_CXX_FLAGS_INIT}")

set(CMAKE_EXE_LINKER_FLAGS_INIT " -L$ENV{PS2SDK}/ee/lib -T ${PROJECT_SOURCE_DIR}/cmake/Toolchain/highlink.ld")

# from original tools.mk buildsystem
set(CMAKE_SHARED_LINKER_FLAGS_INIT "-nostartfiles -nodefaultlibs -Wl,-r -Wl,-d")


set(CMAKE_EXE_LINKER_FLAGS_RELEASE_INIT "-Wl,--gc-sections -s")

#set(CMAKE_STATIC_LINKER_FLAGS_CONFIG_INIT "...")
#set(CMAKE_STATIC_LINKER_FLAGS_INIT "...")

# Suffixes
set(CMAKE_EXECUTABLE_SUFFIX ".elf")
set(CMAKE_EXECUTABLE_SUFFIX_C "${CMAKE_EXECUTABLE_SUFFIX}")
set(CMAKE_EXECUTABLE_SUFFIX_CXX "${CMAKE_EXECUTABLE_SUFFIX}")

set(CMAKE_SHARED_LIBRARY_SUFFIX ".erl")
set(CMAKE_SHARED_LIBRARY_SUFFIX_C "${CMAKE_EXECUTABLE_SUFFIX}")
set(CMAKE_SHARED_LIBRARY_SUFFIX_CXX "${CMAKE_EXECUTABLE_SUFFIX}")

# ??? what the fuck ??? ? Que ?
set(CMAKE_TARGET_INSTALL_PREFIX $ENV{PS2DEV}/ports)

set(CMAKE_FIND_ROOT_PATH $ENV{PS2DEV} $ENV{PS2DEV}/ee $ENV{PS2DEV}/ee/ee $ENV{PS2SDK} $ENV{PS2SDK}/ports)
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
