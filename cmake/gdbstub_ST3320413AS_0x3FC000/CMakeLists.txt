# Copyright 2013-2014 Jonas Zaddach <zaddach@eurecom.fr>, EURECOM
#
# You can redistribute and/or modify this program under the terms of the
# GNU General Public License version 2 or later.

# Build file for the Seagate ST3320413AS hard drive.


SET (STUB_ASSEMBLER_FILES
    src/lowlevel_arm.S
    src/lowlevel_ST3320413AS.S
    src/utils_arm.S
    )

SET (STUB_ARM_FILES
    )

SET (STUB_THUMB_FILES
    src/GdbHostInterface.c
    src/utils.c 
    src/Serial_uart16550.c
    src/ArmRegisterMap.c
    src/ArmGdbHelpers.c
    src/Memory.c
    src/stub.c
    src/crc.c
    )

SET (STUB_NAME gdbstub_ST3320413AS_0x3FC000)
SET (STUB_LINKER_FILE ${PROJECT_SOURCE_DIR}/link/arm_ST3320413AS_0x3FC000.ld)
SET (STUB_CONFIGURATION_HEADER ${PROJECT_SOURCE_DIR}/include/ST3320413AS_gdb_config.h)
SET (TARGET_ARCH armv5t)

BUILD_DEBUG_STUB()
