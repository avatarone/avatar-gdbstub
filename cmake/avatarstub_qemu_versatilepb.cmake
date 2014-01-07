# Copyright 2013-2014 Jonas Zaddach <zaddach@eurecom.fr>, EURECOM
#
# You can redistribute and/or modify this program under the terms of the
# GNU General Public License version 2 or later.


SET (LIBSYSTEM_COMMON_ARM_FILES
    libsystem/src/arm/exception_handler_fiq.c
    libsystem/src/arm/exception_handler_und.c
    libsystem/src/arm/exception_handler_pab.c
    libsystem/src/arm/exception_handler_dab.c
    libsystem/src/arm/exception_handler_irq.c
    libsystem/src/arm/ArmCpu.c
    libsystem/src/arm/exception_handler_svc.c
    libsystem/src/arm/exception_handler_dab.c)

SET (AVATARSTUB_QEMU_VERSATILEPB_ARM_FILES
    src/arm/start.c
    ${LIBSYSTEM_COMMON_ARM_FILES}
    )

SET (AVATARSTUB_COMMON_THUMB_FILES
	src/avatar_protocol.c
	src/avatar_vm.c
	src/avatar_exceptions.c
	src/avatar_memory.c
	src/minivm.c
	src/crc.c
        src/avatar_codelet_api.c
    src/utils.c
#    src/gdb_protocol.c
    )

SET (AVATARSTUB_QEMU_VERSATILEPB_THUMB_FILES
    libsystem/src/hw/serial/Serial_pl011.c
    src/platform_qemu_versatilepb.c)



SET (QEMU_VERSATILEPB_LINKER_FILE ${PROJECT_SOURCE_DIR}/link/arm_qemu_versatilepb.ld)

SET_SOURCE_FILES_PROPERTIES(${AVATARSTUB_QEMU_VERSATILEPB_ARM_FILES} PROPERTIES COMPILE_FLAGS "${CMAKE_C_FLAGS} -marm")
SET_SOURCE_FILES_PROPERTIES(${AVATARSTUB_COMMON_THUMB_FILES} PROPERTIES COMPILE_FLAGS "${CMAKE_C_FLAGS} -mthumb")
SET_SOURCE_FILES_PROPERTIES(${AVATARSTUB_QEMU_VERSATILEPB_THUMB_FILES} PROPERTIES COMPILE_FLAGS "${CMAKE_C_FLAGS} -mthumb")

ADD_EXECUTABLE(avatarstub_qemu_versatilepb ${AVATARSTUB_QEMU_VERSATILEPB_ARM_FILES} ${AVATARSTUB_COMMON_THUMB_FILES} ${AVATARSTUB_QEMU_VERSATILEPB_THUMB_FILES})
SET_TARGET_PROPERTIES(avatarstub_qemu_versatilepb PROPERTIES LINK_FLAGS "-T ${QEMU_VERSATILEPB_LINKER_FILE} -nostdlib -nodefaultlibs")
TARGET_LINK_LIBRARIES(avatarstub_qemu_versatilepb stdlib)
