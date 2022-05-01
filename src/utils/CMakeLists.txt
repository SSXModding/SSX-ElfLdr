
# These sources are in both libutils-elf
# and libutils-erl (the minimized version for the ERL to use.)
set(__ELFLDR_UTILS_BASE_SOURCES
        assert.cpp
        codeutils.cpp
        debugout.cpp
        Hook.cpp
        )

add_library(elfldr_utils_elf
        ${__ELFLDR_UTILS_BASE_SOURCES}
        FioFile.cpp
        )

target_include_directories(elfldr_utils_elf PUBLIC ${PROJECT_SOURCE_DIR}/include/)

# ERL version
add_library(elfldr_utils_erl
        ${__ELFLDR_UTILS_BASE_SOURCES}
        )

target_include_directories(elfldr_utils_erl PUBLIC ${PROJECT_SOURCE_DIR}/include/)


# use these aliases thx :)
add_library(elfldr::utils_elf ALIAS elfldr_utils_elf)
add_library(elfldr::utils_erl ALIAS elfldr_utils_erl)