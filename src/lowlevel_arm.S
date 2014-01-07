/*
 * Copyright 2013-2014 Jonas Zaddach <zaddach@eurecom.fr>, EURECOM
 *
 * You can redistribute and/or modify this program under the terms of the
 * GNU General Public License version 2 or later.
 */

/* Signal numbers taken from: http://opensource.apple.com/source/cxxfilt/cxxfilt-9/cxxfilt/include/gdb/signals.h */

#define MODE_USR  0x10
#define MODE_FIQ  0x11
#define MODE_IRQ  0x12
#define MODE_SVC  0x13
#define MODE_ABRT 0x17
#define MODE_UND  0x1b
#define MODE_SYS  0x1f
#define FIQ_DISABLED 0x40
#define IRQ_DISABLED 0x80
	
    .global _arm_PAB_handler
    .global _arm_DAB_handler
	.global get_code_offset
/*    .global _arm_RST_handler
    .global _arm_UND_handler
    .global _arm_SWI_handler
    .global _arm_IRQ_handler
    .global _arm_FIQ_handler */

    .extern gdb_monitor


.section .header
.arm
b _start
b _arm_PAB_handler
b _arm_DAB_handler

	
.section .text
.arm
        

_arm_PAB_handler:
        /* 
         * Our register file on the stack has the following layout:
         *  -  0: CPSR = SPSR_abrt, SP_abrt
         *  -  2: SPSR_fiq, R8_fiq - R12_fiq, SP_fiq, LR_fiq
         *  - 10: SPSR_irq, SP_irq, LR_irq
         *  - 13: SPSR_svc, SP_svc, LR_svc
         *  - 16: SPSR_und, SP_und, LR_und
         *  - 19: SPSR_sys, SP_sys, LR_sys
         *  - 22: SP_usr, LR_usr
         *  - 24: R0 - R12 in order
         *  - 37: PC = LR_abrt
         *
         * GDB expects the following layout for the register file on the stack:
         *  - R0 - R12 in order
         *  - SP
         *  - LR
         *  - PC
         *  - FP0 - FP7 ; each floating point register is 12 bytes
         *  - CPSR
         * A mapper function handles the conversion from our layout to the GDB layout.
         */
        /* TODO: Disable interrupts */



        /* Save general purpose registers to register file (R0 - R12) */
        sub lr, lr, #4
        push {lr}
        mov lr, #5 /* SIG_TRAP */

        b _abort_handlers

_arm_DAB_handler:
        sub lr, lr, #8
        push {lr}
        mov lr, #6 /* SIG_ABRT */

        b _abort_handlers

_abort_handlers:
        /* Save general purpose registers */
        push {r0-r12}

        /* Save banked registers */
        /* First user mode registers */
        sub r2, sp, #(2 * 4)
        stm r2, {sp, lr}^
        /* No read access to registers unique to this mode for two cycles after access to banked registers */
        nop
        nop
        /* Now registers for all other modes. Abort is last mode and is kept. */
        mov r0, #0
        /* Use r2 instead of sp for stack because sp is unavailable after we switch modes */
        mrs r4, CPSR
Leach_mode:
        adr r1, Lcpu_modes
        ldr r1, [r1, r0, LSL #2]
        /* Switch mode */
        msr CPSR_c, r1
        /* Get SPSR */
        mrs r3, SPSR
        cmp r1, #(MODE_FIQ | IRQ_DISABLED | FIQ_DISABLED)
        /* For every mode except FIQ, store SPSR, SP, LR */
        stmnedb r2!, {r3, sp, lr}
        /* For FIQ store SPSR, R9-R12, SP, LR */
        stmeqdb r2!, {r3, r8-r12, sp, lr}
        add r0, r0, #1
        cmp r0, #5
        blo Leach_mode

        /* Save ABRT registers */
        msr CPSR, r4

        mrs r3, SPSR
        stmdb r2!, {r3, sp}
        
        /* update stack pointer */
        mov sp, r2

        mov r0, lr
        mov r1, sp

        /* gdb_monitor(address_t pc, void * register_file) */
        blx stub_handle_exception

        /* Restore banked registers */
        /* First ABRT mode SPSR */
        pop {r1, r2}
        msr SPSR, r1

        /* Now the other modes */
        mov r0, #4
        mov r2, sp
        mrs r4, CPSR
 Leach_mode2:
        adr r1, Lcpu_modes
        ldr r1, [r1, r0, LSL #2] 
        msr CPSR_c, r1  
        cmp r1, #(MODE_FIQ | IRQ_DISABLED | FIQ_DISABLED)
        /* For every mode except FIQ and ABRT, restore SPSR, SP, LR */
        ldmneia r2!, {r3, sp, lr}
        /* For FIQ restore SPSR, R9-R12, SP, LR */
        ldmeqia r2!, {r3, r8-r12, sp, lr}
        msr SPSR, r3
        subs r0, r0, #1
        bpl Leach_mode2

        msr CPSR_c, r4
        

        /* Now restore user mode registers */
        ldm r2, {sp, lr}^
        nop
        nop
        add sp, r2, #(2 * 4)

        ldmia sp!, {r0 - r12, pc}^

.align 4
Lcpu_modes:
    .long (MODE_SYS | IRQ_DISABLED | FIQ_DISABLED)
    .long (MODE_UND | IRQ_DISABLED | FIQ_DISABLED)
    .long (MODE_SVC | IRQ_DISABLED | FIQ_DISABLED)
    .long (MODE_IRQ | IRQ_DISABLED | FIQ_DISABLED)
    .long (MODE_FIQ | IRQ_DISABLED | FIQ_DISABLED)
        
/* Uses only register r0 */
get_code_offset:        
        ldr r0, =get_code_offset+12
        sub r0, r0, pc
        bx lr
/* uint32_t get_banked_register(int mode_num, int register) */
get_banked_register:
        cmp r0, #0x10
        bne Lnot_user_mode
        sub sp, sp, #4
        cmp r1, #0
        stmeq sp, {sp}^
        stmne sp, {lr}^
        pop {r0}
        bx lr
Lnot_user_mode:
        mrs r2, CPSR
        orr r0, #0xC0
        msr CPSR_c, r0
        cmp r1, #0
        moveq r0, sp
        cmp r1, #1
        moveq r0, lr
        mrsne r0, SPSR
        msr CPSR, r2
        bx lr

        

        
        
