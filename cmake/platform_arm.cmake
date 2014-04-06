# Copyright 2013-2014 Jonas Zaddach <zaddach@eurecom.fr>, EURECOM
#
# You can redistribute and/or modify this program under the terms of the
# GNU General Public License version 2 or later.

# the name of the target operating system
SET(CMAKE_SYSTEM_NAME Generic)
SET(CMAKE_SYSTEM_PROCESSOR arm)

# which compilers to use for C and C++
SET(CMAKE_C_COMPILER arm-none-eabi-gcc)
SET(CMAKE_CXX_COMPILER arm-none-eabi-g++)
SET(CMAKE_ASM_COMPILER arm-none-eabi-gcc)
SET(CMAKE_SIZE arm-none-eabi-size)
SET(CMAKE_OBJCOPY arm-none-eabi-objcopy)
SET(CMAKE_AR arm-none-eabi-ar)
SET(CMAKE_RANLIB arm-none-eabi-ranlib)

# adjust the default behaviour of the FIND_XXX() commands:
# search headers and libraries in the target environment, search 
# programs in the host environment
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

#Remove switches that prevent inclusion of symbol table in binary
SET (CMAKE_C_LINK_FLAGS "")

MACRO(BUILD_DEBUG_STUB)
	ADDPREFIX(STUB_ASSEMBLER_FILES ../../)
	ADDPREFIX(STUB_ARM_FILES ../../)
	ADDPREFIX(STUB_THUMB_FILES ../../)

	SET (CUSTOM_FLAGS)
	IF (DEFINED TARGET_ARCH)
		SET(CUSTOM_FLAGS "${CUSTOM_FLAGS} -march=${TARGET_ARCH}")
	ENDIF (DEFINED TARGET_ARCH)
	IF (DEFINED TARGET_CPU)
		SET(CUSTOM_FLAGS "${CUSTOM_FLAGS} -mcpu=${TARGET_CPU}")
	ENDIF (DEFINED TARGET_CPU)
	IF (DEFINED TARGET_ENDIAN)
		SET(CUSTOM_FLAGS "${CUSTOM_FLAGS} -m${TARGET_ENDIAN}")
	ENDIF (DEFINED TARGET_ENDIAN)



	SET_SOURCE_FILES_PROPERTIES(${STUB_ARM_FILES} PROPERTIES COMPILE_FLAGS "${CMAKE_C_FLAGS} -include ${STUB_CONFIGURATION_HEADER} -marm ${CUSTOM_FLAGS}")
	SET_SOURCE_FILES_PROPERTIES(${STUB_THUMB_FILES} PROPERTIES COMPILE_FLAGS "${CMAKE_C_FLAGS} -include ${STUB_CONFIGURATION_HEADER} -mthumb ${CUSTOM_FLAGS}")
	SET_SOURCE_FILES_PROPERTIES(${STUB_ASSEMBLER_FILES} PROPERTIES COMPILE_FLAGS "${CMAKE_ASM_FLAGS} ${CUSTOM_FLAGS}")
	

	ADD_EXECUTABLE(${STUB_NAME} ${STUB_ARM_FILES} ${STUB_THUMB_FILES} ${STUB_ASSEMBLER_FILES})
	SET_TARGET_PROPERTIES(${STUB_NAME} PROPERTIES LINK_FLAGS "-T ${STUB_LINKER_FILE} -nostdlib -nodefaultlibs ${CUSTOM_FLAGS}")

	ADD_CUSTOM_COMMAND(OUTPUT ${STUB_NAME}.bin.tmp
	                   COMMAND arm-none-eabi-objcopy --output-format=binary ${STUB_NAME} ${STUB_NAME}.bin.tmp
	                   COMMAND ${CMAKE_COMMAND} -E copy ${STUB_NAME}.bin.tmp ${STUB_NAME}.bin
	                   DEPENDS ${STUB_NAME}
	                   COMMENT "objcopy elf to bin")
	ADD_CUSTOM_TARGET(${STUB_NAME}.bin ALL DEPENDS ${STUB_NAME}.bin.tmp)
	SET_DIRECTORY_PROPERTIES(PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES ${STUB_NAME}.bin)
ENDMACRO(BUILD_DEBUG_STUB)
