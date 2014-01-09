# Copyright 2013-2014 Jonas Zaddach <zaddach@eurecom.fr>, EURECOM
#
# You can redistribute and/or modify this program under the terms of the
# GNU General Public License version 2 or later.

PROJECT(stub C ASM)
CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/include
                    ${PROJECT_BINARY_DIR})

INCLUDE(cmake/platform_arm.cmake)

SET(CMAKE_SHARED_LIBRARY_LINK_C_FLAGS "") #Suppress -rdynamic in linker flags

SET (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -march=armv5t -mno-thumb-interwork -fPIC -Os")
SET(CMAKE_ASM_FLAGS "-fPIC -Wall" )

#INCLUDE (cmake/avatarstub_qemu_versatilepb.cmake)
#INCLUDE (cmake/avatarstub_ST3320413AS_0x7000.cmake)
#INCLUDE (cmake/gdbstub_ST3320413AS_0x7000.cmake)
#INCLUDE (cmake/gdbstub_qemu_versatilepb.cmake)
INCLUDE (cmake/gdbstub_plc_bootloader_0x16000.cmake)
