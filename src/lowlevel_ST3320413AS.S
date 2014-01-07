/*
 * Copyright 2013-2014 Jonas Zaddach <zaddach@eurecom.fr>, EURECOM
 *
 * You can redistribute and/or modify this program under the terms of the
 * GNU General Public License version 2 or later.
 */

    .global _start
    .global ST3320413AS_flush_write_buffer
    .type ST3320413AS_flush_write_buffer STT_FUNC

     .global ST3320413AS_disable_instruction_prefetch
    .type ST3320413AS_disable_instruction_prefetch STT_FUNC

     .global ST3320413AS_get_instruction_prefetch
    .type ST3320413AS_get_instruction_prefetch STT_FUNC

    .extern _arm_PAB_handler
    .extern _arm_DAB_handler
	
	.arm
    .section .start
_start:
        mov r3, lr
        /* Install PAB and DAB handler */
        /* First write the target adresses */
        bl get_code_offset
        ldr r2, =_arm_PAB_handler
        add r2, r2, r0
        ldr r1, =_arm_DAB_handler
        add r1, r1, r0

        mov r0, #0xC
        str r2, [r0, #0x20]
        str r1, [r0, #0x24]

        /* And then the LDR instruction */
        ldr r1, =(0xe59ff000 | 0x18)
        str r1, [r0]
        str r1, [r0, #0x04]
        
        /* Hit breakpoint to allow GDB initialization */
        bkpt #0x42 
        bx r3

.text
.arm


/* I doubt we would need something like that one day ...
 maybe timings ? 
 ST3320413AS_enable_instruction_prefetch:
 	mrc p15, 1, r1, c15, c1, 0
         bic r1, #0x10000
         mcr p15, 1, r1, c15, c1, 0
*/	
	
