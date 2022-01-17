add_library(elfldr_erl
	ErlLoader.cpp
)

# techinically unneeded but whatever
target_include_directories(elfldr_erl PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(elfldr_erl PUBLIC ${PROJECT_SOURCE_DIR}/include)

# this would ideally only be linked to ELFs
# so use libutils-elf
#target_link_libraries(elfldr_erl PUBLIC elfldr::utils_elf)

# Use this alias please :)
add_library(elfldr::erl ALIAS elfldr_erl)