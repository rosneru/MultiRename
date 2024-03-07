cmake_minimum_required (VERSION 3.5)

SET(CROSS_COMPILE m68k-amigaos-)
SET(CMAKE_C_COMPILER m68k-amigaos-gcc)

SET(CMAKE_C_FLAGS "-Os -s -pedantic -noixemul -fomit-frame-pointer -fno-exceptions -Wall -Wno-pointer-sign -Wno-int-conversion")

set(EXTRALIBS_LINK_FLAGS "") # Add a static linker lib if needed, e.g. "-lpthread"

set(CMAKE_C_LINK_EXECUTABLE "<CMAKE_C_COMPILER> <FLAGS> <CMAKE_C_LINK_FLAGS> <OBJECTS> -o <TARGET>")
set(CMAKE_C_LINK_EXECUTABLE "${CMAKE_C_LINK_EXECUTABLE} ${EXTRALIBS_LINK_FLAGS}")

add_executable (MultiRename_gcc
                application.c
                arguments.c
                file_node.c
                main.c
                rename_algorithm.c)

add_custom_command(TARGET MultiRename_gcc
                   POST_BUILD
                   COMMAND cp MultiRename_gcc ${CMAKE_CURRENT_SOURCE_DIR}/..
                   DEPENDS MultiRename_gcc)
