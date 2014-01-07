/*
 * Copyright 2013-2014 Jonas Zaddach <zaddach@eurecom.fr>, EURECOM
 *
 * You can redistribute and/or modify this program under the terms of the
 * GNU General Public License version 2 or later.
 */

#ifndef _ARM_REGISTER_MAP_H
#define _ARM_REGISTER_MAP_H

#include <stdint.h>

#define REG_SPSR_ABT 0
#define REG_SP_ABT 1

#define REG_SPSR_FIQ 2
#define REG_R8_FIQ 3
#define REG_R9_FIQ 4
#define REG_R10_FIQ 5
#define REG_R11_FIQ 6
#define REG_R12_FIQ 7
#define REG_SP_FIQ 8
#define REG_LR_FIQ 9

#define REG_SPSR_IRQ 10
#define REG_SP_IRQ 11
#define REG_LR_IRQ 12

#define REG_SPSR_SVC 13
#define REG_SP_SVC 14
#define REG_LR_SVC 15

#define REG_SPSR_UND 16
#define REG_SP_UND 17
#define REG_LR_UND 18

#define REG_SPSR_SYS 19
#define REG_SP_SYS 20
#define REG_LR_SYS 21

#define REG_SP_USR 22
#define REG_LR_USR 23


#define REG_R0 24
#define REG_R1 25
#define REG_R2 26
#define REG_R3 27
#define REG_R4 28
#define REG_R5 29
#define REG_R6 30
#define REG_R7 31
#define REG_R8 32
#define REG_R9 33
#define REG_R10 34
#define REG_R11 35
#define REG_R12 36

#define REG_PC 37
#define REG_CPSR 0


typedef struct
{
    uint32_t spsr_abt; //= CPSR
    uint32_t sp_abt;
    
    uint32_t spsr_fiq;
    uint32_t high_registers[5]; //r8_fiq - r12_fiq
    uint32_t lr_fiq;
    
    uint32_t spsr_irq;
    uint32_t sp_irq;
    uint32_t lr_irq;
    
    uint32_t spsr_svc;
    uint32_t sp_svc;
    uint32_t lr_svc;
    
    uint32_t spsr_und;
    uint32_t sp_und;
    uint32_t lr_und;
    
    uint32_t spsr_sys;
    uint32_t sp_sys;
    uint32_t lr_sys;
    
    uint32_t sp_usr;
    uint32_t lr_usr;
    
    uint32_t registers[13]; //r0 - r12
    uint32_t pc;
} __attribute__((__packed__)) RegisterMap;

#endif /* _ARM_REGISTER_MAP_H */
