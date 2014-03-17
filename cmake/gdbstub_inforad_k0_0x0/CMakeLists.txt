# Copyright 2013-2014 Jonas Zaddach <zaddach@eurecom.fr>, EURECOM
#
# You can redistribute and/or modify this program under the terms of the
# GNU General Public License version 2 or later.

# Build file for the Inforad K0 SIR III GPS stick


SET (STUB_ASSEMBLER_FILES
    src/lowlevel_arm.S
    src/lowlevel_inforad_k0.S
    src/utils_arm.S
    )

SET (STUB_THUMB_FILES
    )

SET (STUB_ARM_FILES
    src/GdbHostInterface.c
#	src/gdbstub_arm.c
    src/utils.c 
    src/Serial_sirf.c
    src/ArmRegisterMap.c
    src/ArmGdbHelpers.c
    src/Memory.c
    src/stub.c
	src/crc.c
    )

SET (STUB_NAME gdbstub_inforad_k0_0x0)
SET (STUB_LINKER_FILE ${PROJECT_SOURCE_DIR}/link/arm_inforad_k0_0x0.ld)
SET (STUB_CONFIGURATION_HEADER ${PROJECT_SOURCE_DIR}/include/inforad_k0_gdb_config.h)
SET (TARGET_CPU arm7tdmi)
SET (TARGET_ENDIAN little-endian)

BUILD_DEBUG_STUB()
