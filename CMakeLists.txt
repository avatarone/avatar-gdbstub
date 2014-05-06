# Copyright 2013-2014 Jonas Zaddach <zaddach@eurecom.fr>, EURECOM
#
# You can redistribute and/or modify this program under the terms of the
# GNU General Public License version 2 or later.

PROJECT(stub C ASM)
CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

FUNCTION(ADDPREFIX list_name prefix)
    # create empty list - necessary?
    SET(${list_name}_TMP)
 
    # prefix and suffix elements
    foreach(l ${${list_name}})
      list(APPEND ${list_name}_TMP ${prefix}${l} )
    endforeach()
 
    # replace list by tmp list
    SET(${list_name} ${${list_name}_TMP} PARENT_SCOPE)
    UNSET(${list_name}_TMP)
ENDFUNCTION(ADDPREFIX)

INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/include
                    ${PROJECT_BINARY_DIR})

INCLUDE(cmake/platform_arm.cmake)

SET(CMAKE_SHARED_LIBRARY_LINK_C_FLAGS "") #Suppress -rdynamic in linker flags

SET (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -mno-thumb-interwork -fPIC")
SET(CMAKE_ASM_FLAGS "-fPIC -Wall" )

#ADD_SUBDIRECTORY (cmake/avatarstub_qemu_versatilepb)
#ADD_SUBDIRECTORY (cmake/avatarstub_ST3320413AS_0x7000)
ADD_SUBDIRECTORY (cmake/gdbstub_ST3320413AS_0x7000)
ADD_SUBDIRECTORY (cmake/gdbstub_ST3320413AS_0x3FC000)
ADD_SUBDIRECTORY (cmake/gdbstub_qemu_versatilepb)
ADD_SUBDIRECTORY (cmake/gdbstub_plc_bootloader_0x16000)
ADD_SUBDIRECTORY (cmake/gdbstub_inforad_k0_0x0)
