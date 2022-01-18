
add_executable(elfldr
	# Elfldr core
	ElfLoader.cpp
	GameVersion.cpp
	main.cpp
	patch.cpp

	# patches
	patch_hostfs.cpp
	patch_memory.cpp
	patch_experimental.cpp
)

target_include_directories(elfldr PRIVATE ${PROJECT_BINARY_DIR} ${CMAKE_CURRENTS_SOURCE_DIR})
add_dependencies(elfldr __elfldr_gittag)

# -Wl,--start-group -lc_nano -lkernel -lps2sdkc -Wl,--end-group

# This ugliness is so we don't link anything we don't need.
# We also prefer/use nano Newlib, to make the executable even smaller.
target_link_libraries(elfldr
	"-nodefaultlibs"
	"-Wl,--start-group"
	# System libraries
	c_nano
	kernel
	ps2sdkc

	# Project libraries
	elfldr::utils_elf
	elfldr::erl
	"-Wl,--end-group")

# Make elfldr output in the root of the build directory, mostly for ease.
set_target_properties(elfldr PROPERTIES
	RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
)