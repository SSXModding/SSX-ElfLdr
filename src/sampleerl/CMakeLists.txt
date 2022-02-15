
# TODO: Need to get this working to test hook code

add_library(sample_erl SHARED
        sampleerl.cpp
        )

set_target_properties(sample_erl PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
        POSITION_INDEPENDENT_CODE OFF
        )

#set_property(GLOBAL PROPERTY POSITION_INDEPENDENT_CODE OFF)

target_link_libraries(sample_erl PUBLIC
        elfldr::runtime
        elfldr::utils_erl
        )