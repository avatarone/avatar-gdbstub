# Copyright 2013-2014 Lucian Cojocar <lucian.cojocar@vu.nl>, VU
#
# You can redistribute and/or modify this program under the terms of the
# GNU General Public License version 2 or later.

SET (STUB_ASSEMBLER_FILES
	src/lowlevel_arm.S
	src/lowlevel_plc.S
	)

SET (STUB_ARM_FILES
	src/GdbHostInterface.c
	#	src/gdbstub_arm.c
	src/crc.c
	src/utils.c 
	src/Serial_pl011.c
	src/ArmRegisterMap.c
	src/ArmGdbHelpers.c
	src/Memory.c
	src/stub.c
	src/SerialIO.c
	src/armv7_cortex_r4.c
	)

SET (STUB_NAME gdbstub_plc_bootloader_0x16000)
SET (STUB_LINKER_FILE ${PROJECT_SOURCE_DIR}/link/arm_plc_bootloader_0x16000.ld)
SET (STUB_CONFIGURATION_HEADER ${PROJECT_SOURCE_DIR}/include/plc_bootloader_gdb_config.h)

########### BELOW IS COMMON STUFF ###############################################

# don't use the predefined flags
SET_SOURCE_FILES_PROPERTIES(${STUB_ARM_FILES} PROPERTIES COMPILE_FLAGS "-include ${STUB_CONFIGURATION_HEADER} -marm -march=armv7-r -mbig-endian -mno-thumb-interwork -Os -fPIC -Wall")

# compile the stub as big endian
SET_SOURCE_FILES_PROPERTIES(${STUB_ASSEMBLER_FILES} PROPERTIES COMPILE_FLAGS "-marm -march=armv7-r -mbig-endian -fPIC -Wall")


ADD_EXECUTABLE(${STUB_NAME} ${STUB_ARM_FILES} ${STUB_ASSEMBLER_FILES})
SET_TARGET_PROPERTIES(${STUB_NAME} PROPERTIES LINK_FLAGS "-T ${STUB_LINKER_FILE} -nostdlib -nodefaultlibs")

add_custom_command(OUTPUT ${STUB_NAME}.bin.tmp
                   COMMAND arm-none-eabi-objcopy --output-format=binary ${STUB_NAME} ${STUB_NAME}.bin.tmp
                   COMMAND ${CMAKE_COMMAND} -E copy ${STUB_NAME}.bin.tmp ${STUB_NAME}.bin
                   DEPENDS ${STUB_NAME}
                   COMMENT "objcopy elf to bin")
add_custom_target(${STUB_NAME}.bin ALL DEPENDS ${STUB_NAME}.bin.tmp)
set_directory_properties(PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES ${STUB_NAME}.bin)
