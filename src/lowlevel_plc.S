/*
 * Copyright 2013-2014 Lucian Cojocar <lucian.cojocar@vu.nl>, VU
 *
 * You can redistribute and/or modify this program under the terms of the
 * GNU General Public License version 2 or later.
 */

.global _start

.extern _arm_PAB_handler
.extern _arm_DAB_handler
.extern _gdb_stub_start

.arm
.section .start

/* start is at 0x16000 */
/* jmp to our stub is at 0xe0000 */
/* we should return at 0xe0000+16 */
#define JMP_ADDR 0xE000
#define RET_ADDR 0xE010

_start:
	/* assume that we have a valid stack */
	push {r0, r1, r2, r3, r4, r5, r6, r7, lr}

	/* Install PAB and DAB handler */

	/* load pointers to handlers */
	ldr r2, =_arm_PAB_handler
	ldr r1, =_arm_DAB_handler

	/* at 0xbc we have the Exception vector pointers */
	/* we can override these values to execute our handlers */
	mov r0, $0xbc
	str r2, [r0, $0x0c] /* abort prefetch (PAB) */
	#str r1, [r0, $0x10] /* abort data (DAB) */


	/* Hit breakpoint to allow GDB initialization */
	bkpt #0x42

	pop {r0, r1, r2, r3, r4, r5, r6, r7, lr}

	/* continue the boot process */
	b RET_ADDR
