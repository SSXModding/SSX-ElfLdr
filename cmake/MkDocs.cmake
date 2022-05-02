#
# SSX-Elfldr
#
# (C) 2021-2022 Lily/modeco80 <lily.modeco80@protonmail.ch>
# under the terms of the MIT license.
#

# Semi-hack to build documentation.

find_package(MkDocs)

if(MkDocs_FOUND)
    set(DOCS_ENABLED TRUE)
    message(STATUS "Enabling documentation build")
    add_custom_target(docs ALL ${MKDOCS_EXECUTABLE} build
            WORKING_DIRECTORY ..
            COMMENT "Generating HTML documentation" VERBATIM)
else()
    set(DOCS_ENABLED FALSE)
endif()
