/*
 * Copyright 2013-2014 Jonas Zaddach <zaddach@eurecom.fr>, EURECOM
 *
 * You can redistribute and/or modify this program under the terms of the
 * GNU General Public License version 2 or later.
 */

#include "ArmGdbHelpers.h"
#include "RegisterMap.h"
#include "ArmRegisterMap.h"
#include "Assert.h"

unsigned Gdb_map_gdb_register_number_to_stub(StubState *state, unsigned reg)
{
    if (reg >= 0 && reg <= 12) //R0 -R12
        return REG_R0 + reg;
    else if (reg >= 13 && reg <= 14)
    {
        uint32_t spsr = RegisterMap_get_register(state, REG_SPSR_ABT);
        switch (spsr & 0x1F)
        {
            case 0x10: //USR 
                if (reg == 13)
                    return REG_SP_USR;
                else
                    return REG_LR_USR;
            case 0x11: //FIQ
                if (reg == 13)
                    return REG_SP_FIQ;
                else
                    return REG_LR_FIQ;
            case 0x12: //IRQ
                if (reg == 13)
                    return REG_SP_IRQ;
                else
                    return REG_LR_IRQ;
            case 0x13: //SVC
                if (reg == 13)
                    return REG_SP_SVC;
                else
                    return REG_LR_SVC;
            case 0x17: //ABRT
                if (reg == 13)
                    return REG_SP_ABT;
                else
                    return REG_PC;
            case 0x1b: //UND
                if (reg == 13)
                    return REG_SP_UND;
                else
                    return REG_LR_UND;
            case 0x1f: //SYS
                 if (reg == 13)
                    return REG_SP_SYS;
                else
                    return REG_LR_SYS;  
            default: //Unkown mode
                return 0;
        }
    }
    else
    {
        switch (reg)
        {
            case 15: //PC or LR_abrt
            case 51:
                return REG_PC;
            case 25: //SPSR_abrt == CPSR of previous mode
            case 49:
                return REG_SPSR_ABT;
            case 27:
                return REG_SP_USR;
            case 28:
                return REG_LR_USR;
            case 29:
                return REG_SPSR_FIQ;
            case 30:
            case 31:
            case 32:
            case 33:
            case 34:
                return REG_R8_FIQ + reg - 30;
            case 35:
                return REG_SP_FIQ;
            case 36:
                return REG_LR_FIQ;
            case 37:
                return REG_SPSR_IRQ;
            case 38:
                return REG_SP_IRQ;
            case 39:
                return REG_LR_IRQ;
            case 40:
                return REG_SPSR_SVC;
            case 41:
                return REG_SP_SVC;
            case 42:
                return REG_LR_SVC;
            case 43:
                return REG_SPSR_UND;
            case 44:
                return REG_SP_UND;
            case 45:
                return REG_LR_UND;
            case 46:
                return REG_SPSR_SYS;
            case 47:
                return REG_SP_SYS;
            case 48:
                return REG_LR_SYS;
            case 50:
                return REG_SP_ABT;
        }
    }

    assert(0 && "Unknown GDB register number");
    return INVALID_REGISTER;
}

void Gdb_continue_execution(StubState *state)
{
    register_t pc = RegisterMap_get_register(state, REG_PC);
    register_t cpsr = RegisterMap_get_register(state, REG_SPSR_ABT);

    if (cpsr & 0x20) //Thumb
    {
        if ((*((uint16_t *) pc) & 0xFF00) == 0xBE00) //Thumb bkpt instruction
        {
            RegisterMap_set_register(state, REG_PC, pc + 2);
        }
    }
    else
    {
        if ((*((uint32_t *) pc) & 0xFFF000F0) == 0xE1200070) //ARM bkpt instruction
        {
            RegisterMap_set_register(state, REG_PC, pc + 4);
        }
    }
}
