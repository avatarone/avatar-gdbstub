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
    .extern _arm_UND_handler
	
	.arm
    .section .init
_start:
		/* The exception vector table */
		b _reset /* RST */
		ldr pc, =_arm_UND_handler /* UND */
		b .      /* SVC */
		ldr pc, =_arm_PAB_handler /* PAB */
		ldr pc, =_arm_DAB_handler /* DAB */
		nop /* Reserved */
		b . /* IRQ */
		b . /* FIQ */

_reset:
		mrs r0, CPSR
		mov r1, #0x1f
		mvn r1, r1
		and r2, r0, r1
		orr r2, r2, #0x1b
		msr CPSR, r2
		ldr sp, =0x10000
		msr CPSR, r0
		ldr sp, =0x20000

		b _arm_UND_handler
        /* Hit breakpoint to allow GDB initialization */
        .long 0xe7f001f0
        /* bkpt #0x42 */
        b _reset


/* I doubt we would need something like that one day ...
 maybe timings ? 
 ST3320413AS_enable_instruction_prefetch:
 	mrc p15, 1, r1, c15, c1, 0
         bic r1, #0x10000
         mcr p15, 1, r1, c15, c1, 0
*/	
	
