# Copyright 2013-2014 Lucian Cojocar <lucian.cojocar@vu.nl>, VU
#
# You can redistribute and/or modify this program under the terms of the
# GNU General Public License version 2 or later.

# Build file for the Siemens S7-1200 PLC (6E7S212-1BE31-0XB0)

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
SET (TARGET_ARCH armv7-r)
SET (TARGET_ENDIAN big-endian)

BUILD_DEBUG_STUB()