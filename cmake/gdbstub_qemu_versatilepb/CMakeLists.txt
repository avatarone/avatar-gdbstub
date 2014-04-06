# Copyright 2013-2014 Jonas Zaddach <zaddach@eurecom.fr>, EURECOM
#
# You can redistribute and/or modify this program under the terms of the
# GNU General Public License version 2 or later.

# Build file for the Qemu VersatilePB platform.

SET (STUB_ASSEMBLER_FILES
    src/lowlevel_arm.S
    src/lowlevel_qemu_versatilepb.S
    src/utils_arm.S)

SET (STUB_ARM_FILES
	src/utils.c
    src/GdbHostInterface.c
    src/utils.c 
    src/Serial_pl011.c
    src/ArmRegisterMap.c
    src/ArmGdbHelpers.c
    src/Memory.c
    src/stub.c
    src/crc.c
    )

SET (STUB_THUMB_FILES
    )

SET (STUB_NAME gdbstub_qemu_versatilepb)
SET (STUB_LINKER_FILE ${PROJECT_SOURCE_DIR}/link/arm_qemu_versatilepb.ld)
SET (STUB_CONFIGURATION_HEADER ${PROJECT_SOURCE_DIR}/include/qemu_versatilepb_gdb_config.h)
SET (TARGET_ARCH armv5t)

BUILD_DEBUG_STUB()