#
# SSX-Elfldr
#
# (C) 2021-2022 Lily/modeco80 <lily.modeco80@protonmail.ch>
# under the terms of the MIT license.
#

add_executable(sample_erl
        sampleerl.cpp
        )

# TODO(sdk): It would be really nice to make a function which removes this ugliness
#           and allows you to just make an executable target an erl with one command.
#           One may only wish.

set_target_properties(sample_erl PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
        COMPILE_FLAGS "-fvisibility=hidden"
        LINK_FLAGS "-nostartfiles -nodefaultlibs -u start -Wl,-r -Wl,-d"
        SUFFIX ".erl"
        )

target_link_libraries(sample_erl PUBLIC
        elfldr::runtime
        elfldr::utils_erl
        # Apparently this ps2sdk library doesn't require bringing libc with it.
        # Cool. At least I won't have to do *that* by hand :V
        kernel
        )