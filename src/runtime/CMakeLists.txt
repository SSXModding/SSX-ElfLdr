
add_library(elfldr_runtime

		# C runtime code/library implementaion
		# crt/
		crt/ps2sdk_stubs.cpp
		crt/printf.cpp

		# C++ Runtime code
		Allocator.cpp
		String.cpp
		Fnv1a.cpp
)

# if I didn't need to manually compose a linker argument thing in elfldr itself
# (might be able to subside that soon, to be honest) this would be a good place
# to change to include/runtime
# so that code can just include <String.h>
target_include_directories(elfldr_runtime PUBLIC ${PROJECT_SOURCE_DIR}/include/)

# use these aliases thx :)
add_library(elfldr::runtime ALIAS elfldr_runtime)