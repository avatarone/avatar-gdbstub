/*
 * Copyright 2013-2014 Jonas Zaddach <zaddach@eurecom.fr>, EURECOM
 *
 * You can redistribute and/or modify this program under the terms of the
 * GNU General Public License version 2 or later.
 */

    .global _start

    .extern _arm_PAB_handler
    .extern _arm_DAB_handler
    .extern _arm_UND_handler
	
	.arm
    .section .exception_table
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

	.arm
	.section .init
		
_reset:
		/* Set up stack for SVC mode */
		ldr sp, =0x60017ffc

		/* Set up stack for UND mode */
		mrs r0, CPSR
		mov r1, #0x1f
		mvn r2, r1
		and r3, r0, r2
		orr r3, r3, #0x1b
		msr CPSR, r3
		ldr sp, =0x60016ffc
		msr CPSR, r0

		bl Serial_write_byte
        
        /* Hit breakpoint to allow GDB initialization */
		.long 0xe7f001f0

		b .
