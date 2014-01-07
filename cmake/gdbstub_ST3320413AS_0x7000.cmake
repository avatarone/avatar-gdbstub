# Copyright 2013-2014 Jonas Zaddach <zaddach@eurecom.fr>, EURECOM
#
# You can redistribute and/or modify this program under the terms of the
# GNU General Public License version 2 or later.


SET (STUB_ASSEMBLER_FILES
    src/lowlevel_arm.S
    src/lowlevel_ST3320413AS.S
    src/utils_arm.S
    )

SET (STUB_ARM_FILES
    )

SET (STUB_THUMB_FILES
    src/GdbHostInterface.c
#	src/gdbstub_arm.c
    src/utils.c 
    src/Serial_uart16550.c
    src/ArmRegisterMap.c
    src/ArmGdbHelpers.c
    src/Memory.c
    src/stub.c
    )

SET (STUB_NAME gdbstub_ST3320413AS_0x7000)
SET (STUB_LINKER_FILE ${PROJECT_SOURCE_DIR}/link/arm_ST3320413AS_0x7000.ld)
SET (STUB_CONFIGURATION_HEADER ${PROJECT_SOURCE_DIR}/include/ST3320413AS_gdb_config.h)

########### BELOW IS COMMON STUFF ###############################################

SET_SOURCE_FILES_PROPERTIES(${STUB_ARM_FILES} PROPERTIES COMPILE_FLAGS "${CMAKE_C_FLAGS} -include ${STUB_CONFIGURATION_HEADER} -marm")
SET_SOURCE_FILES_PROPERTIES(${STUB_THUMB_FILES} PROPERTIES COMPILE_FLAGS "${CMAKE_C_FLAGS} -include ${STUB_CONFIGURATION_HEADER} -mthumb")

# ADD_CUSTOM_COMMAND(OUTPUT config.h
#                    COMMAND ${CMAKE_COMMAND} -E copy ${STUB_CONFIGURATION_HEADER} ${PROJECT_BINARY_DIR}/config.h
#                    DEPENDS ${STUB_CONFIGURATION_HEADER}
#                    COMMENT "copy config.h")
# ADD_CUSTOM_TARGET(config.h ALL DEPENDS ${STUB_CONFIGURATION_HEADER})


ADD_EXECUTABLE(${STUB_NAME} ${STUB_ARM_FILES} ${STUB_THUMB_FILES} ${STUB_ASSEMBLER_FILES})
SET_TARGET_PROPERTIES(${STUB_NAME} PROPERTIES LINK_FLAGS "-T ${STUB_LINKER_FILE} -nostdlib -nodefaultlibs")

add_custom_command(OUTPUT ${STUB_NAME}.bin.tmp
                   COMMAND arm-none-eabi-objcopy --output-format=binary ${STUB_NAME} ${STUB_NAME}.bin.tmp
                   COMMAND ${CMAKE_COMMAND} -E copy ${STUB_NAME}.bin.tmp ${STUB_NAME}.bin
                   DEPENDS ${STUB_NAME}
                   COMMENT "objcopy elf to bin")
add_custom_target(${STUB_NAME}.bin ALL DEPENDS ${STUB_NAME}.bin.tmp)
set_directory_properties(PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES ${STUB_NAME}.bin)
