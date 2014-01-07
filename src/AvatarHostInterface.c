/*
 *  Created on: Apr 19, 2013
 *
 * Copyright 2013-2014 Jonas Zaddach <zaddach@eurecom.fr>, EURECOM
 *
 * You can redistribute and/or modify this program under the terms of the
 * GNU General Public License version 2 or later.
 */

#include "AvatarHostInterface.h"
#include "Serial.h"
#include "StubState.h"
#include "crc.h"
#include "Memory.h"
#include "RegisterMap.h"

#define SERIAL_BREAK 0x100


static int recv_byte_unescape()
{
    int val;

    val = Serial_read_byte_blocking();

    if (val == 0x55)
        return SERIAL_BREAK;
    else if (val == 0xAA)
    {
        int second_char = Serial_read_byte_blocking();

        if (second_char == -1)
            return -1;
        else if (second_char == 0x01)
            return 0x55;
        else if (second_char == 0x02)
            return 0xAA;
        else
            /* Error: Unknown escape code */
            return -1;
    }
    else
        return val;
}

static int send_byte_escape(int val)
{
    if (val == SERIAL_BREAK)
        return Serial_write_byte(0x55);
    else
    {
        if (val == 0x55)
        {
            if (Serial_write_byte(0xAA))
                return -1;
            return Serial_write_byte(0x01);
        }
        else if (val == 0xAA)
        {
            if (Serial_write_byte(0xAA))
                return -1;
            return Serial_write_byte(0x02);
        }
        else
        {
            return Serial_write_byte(val);
        }
    }

}


static uint8_t recv_pull_uint8(StubState *state)
{
    int val = -1;

    if (!state->host_interface.recv_error)
    	val = recv_byte_unescape();
    
    if (val == -1)
        state->host_interface.recv_error = 1;
       
    crc_calc(&state->host_interface.recv_checksum, val); 
        
    return (uint8_t) val;
}

static int recv_start(StubState *state)
{
    state->host_interface.recv_checksum = 0;
    state->host_interface.recv_error = 0;

    return recv_pull_uint8(state);
}

static uint16_t recv_pull_uint16(StubState *state)
{
    uint16_t lower = recv_pull_uint8(state);
    uint16_t higher = recv_pull_uint8(state);
    
    return ((higher << 8) | lower);
}

static uint32_t recv_pull_uint32(StubState *state)
{
    uint32_t lower = recv_pull_uint16(state);
    uint32_t higher = recv_pull_uint16(state);
    
    return (higher << 16) | lower;
}

static int recv_verify_checksum(StubState *state)
{
    if (state->host_interface.recv_error)
        return 0;
        
    recv_pull_uint8(state);
    return state->host_interface.recv_checksum == 0;
}


static void send_push_uint8(StubState *state, uint8_t val)
{
    if (send_byte_escape(val))
        state->host_interface.send_error = 1;

    crc_calc(&state->host_interface.send_checksum, val);
}

static void send_start_message(StubState *state, int msg)
{
    state->host_interface.send_checksum = 0;
    state->host_interface.send_error = 0;
    
    send_push_uint8(state, (uint8_t) msg);
}

static void send_push_uint16(StubState *state, uint16_t val)
{
    send_push_uint8(state, val & 0xff);
    send_push_uint8(state, val >> 8);
}

static void send_push_uint32(StubState *state, uint32_t val)
{
    send_push_uint16(state, val & 0xffff);
    send_push_uint16(state, val >> 16);
}

static void send_end_message(StubState *state)
{
    send_byte_escape(state->host_interface.send_checksum);
    send_byte_escape(SERIAL_BREAK);
}

static void send_error_message(StubState *state, AvatarError err)
{
	send_start_message(state, AVATAR_RPC_DTH_REPLY_ERROR);
	send_push_uint8(state, err);
	send_end_message(state);
}

// static void send_ok_message(StubState *state)
// {
// 	send_start_message(state, AVATAR_RPC_DTH_REPLY_OK);
// 	send_end_message(state);
// }

void Avatar_report_state(StubState *state, AvatarVmState vm_state)
{
	send_start_message(state, AVATAR_RPC_DTH_STATE);
	send_push_uint8(state, vm_state);
	send_end_message(state);
}

void Avatar_report_exception(StubState *state, AvatarProcessorException exc)
{
	send_start_message(state, AVATAR_RPC_DTH_INFO_EXCEPTION);
	send_push_uint8(state, exc);
	send_end_message(state);
}

// void Avatar_handle_pagefault(uint32_t address, uint32_t size)
// {
//     int exit_loop = 0;
// 
//     send_start_message(AVATAR_RPC_DTH_PAGEFAULT);
//     //TODO: Page bits hardcoded (6)
//     send_push_uint32(address & (0xFFFFFFFF << 6));
//     send_end_message();
// 
//     while (!exit_loop)
//     {
//         int cmd = recv_start();
//         /* TODO: Error handling, check cmd != -1 */
//         switch (cmd)
//         {
//             case AVATAR_RPC_HTD_INSERT_PAGE:
//             {
//                 uint32_t page_address = recv_pull_uint32();
//                 int i;
//                 uint8_t page[AVATAR_PAGE_SIZE];
// 
//                 for (i = 0; i < AVATAR_PAGE_SIZE; i++)
//                 {
//                     page[i] = recv_pull_uint8();
//                 }
// 
//                 if (recv_verify_checksum())
//                 {
//                     send_ok_message();
//                     Avatar_memory_insert_page(page_address, page);
//                 }
//                 else
//                 {
//                     send_error_message(AVATAR_ERROR_CHECKSUM);
//                 }
//                 break;
//             }
//             case AVATAR_RPC_HTD_CONTINUE_FROM_PAGEFAULT:
//             {
//                 if (recv_verify_checksum())
//                 {
//                     exit_loop = 1;
//                 }
//                 else
//                 {
//                     send_error_message(AVATAR_ERROR_CHECKSUM);
//                 }
//                 break;
//             }
//             case AVATAR_RPC_HTD_QUERY_STATE:
//             {
//             	if (recv_verify_checksum())
//             	{
//             		Avatar_report_state(Avatar_vm_get_state());
//             	}
//             	else
//             	{
//             		send_error_message(AVATAR_ERROR_CHECKSUM);
//             	}
//             	break;
//             }
//             default:
//             {
//                 if (recv_verify_checksum())
//                 {
//                     send_start_message(AVATAR_RPC_DTH_PAGEFAULT);
//                     send_push_uint32(address & (0xFFFFFFFF << 6));
//                     send_end_message();
//                 }
//                 else
//                 {
//                     send_error_message(AVATAR_ERROR_CHECKSUM);
//                 }
//                 break;
//             }
//         }
// 
// 
//         while (recv_byte_unescape() != SERIAL_BREAK);
//     }
// }

void HostInterface_init(StubState *state)
{
    //Nothing to do
}

void HostInterface_communicate(StubState *state)
{
    while (1)
    {
        int cmd = recv_start(state);
        /* TODO: Error handling, check cmd != -1 */
        switch (cmd)
        {
            case AVATAR_RPC_HTD_READ_MEMORY:
            {
                uint32_t address = recv_pull_uint32(state);
                uint8_t size = recv_pull_uint8(state);
                if (recv_verify_checksum(state))
                {
                    value_t val = Memory_read_typed(state, address, size);
                    send_start_message(state, AVATAR_RPC_DTH_REPLY_READ_MEMORY);
                    send_push_uint8(state, size);
                    switch (size)
                    {
                        case 1:
                            send_push_uint8(state, val);
                            break;
                        case 2:
                            send_push_uint16(state, val);
                            break;
                        default:
                            send_push_uint32(state, val);
                            break;
                    }
                    send_end_message(state);
                }
                else
                {
                    send_error_message(state, AVATAR_ERROR_CHECKSUM);
                }
                break;
            }
            case AVATAR_RPC_HTD_WRITE_MEMORY:
            {
                uint32_t address = recv_pull_uint32(state);
                uint8_t size = recv_pull_uint8(state);
                uint32_t val;
                
                switch (size)
                {
                    case 1:
                        val = recv_pull_uint8(state);
                        break;
                    case 2:
                        val = recv_pull_uint16(state);
                        break;
                    default:
                        val = recv_pull_uint32(state);
                        break;
                }
                
                if (recv_verify_checksum(state))
                {
                    send_start_message(state, AVATAR_RPC_DTH_REPLY_OK);
                    Memory_write_typed(state, address, size, val);
                    send_end_message(state);
                }
                else
                {
                    send_error_message(state, AVATAR_ERROR_CHECKSUM);
                }
                break;
            }
            case AVATAR_RPC_HTD_READ_UNTYPED_MEMORY:
            {
                uint32_t address = recv_pull_uint32(state);
                uint8_t size = recv_pull_uint8(state);
                uint8_t i;

                if (recv_verify_checksum(state))
                {
                    send_start_message(state, AVATAR_RPC_DTH_REPLY_READ_UNTYPED_MEMORY);
                    for (i = 0; i < size; i++)
                    {
                        send_push_uint8(state, Memory_read_typed(state, address, SIZE_CHAR));
                    }
                    send_end_message(state);
                }
                else
                {
                    send_error_message(state, AVATAR_ERROR_CHECKSUM);
                }
                break;
            }
            case AVATAR_RPC_HTD_WRITE_UNTYPED_MEMORY:
            {
                uint32_t address = recv_pull_uint32(state);
                uint8_t size = recv_pull_uint8(state);
                uint8_t i;

                for (i = 0; i < size; i++)
                {
                    Memory_write_typed(state, address + i, SIZE_CHAR, recv_pull_uint8(state));
                }

                if (recv_verify_checksum(state))
                {
                    send_start_message(state, AVATAR_RPC_DTH_REPLY_OK);
                    send_end_message(state);
                }
                else
                {
                    send_error_message(state, AVATAR_ERROR_CHECKSUM);
                }
                break;
            }
            case AVATAR_RPC_HTD_GET_REGISTER:
            {
                uint8_t regnum = recv_pull_uint8(state);
    
                if (recv_verify_checksum(state))
                {
                    send_start_message(state, AVATAR_RPC_DTH_REPLY_GET_REGISTER);
                    send_push_uint32(state, RegisterMap_get_register(state, regnum));
                    send_end_message(state);
                }
                else
                {
                    send_error_message(state, AVATAR_ERROR_CHECKSUM);
                }
                break;
            }
            case AVATAR_RPC_HTD_SET_REGISTER:
            {
                uint8_t regnum = recv_pull_uint8(state);
                uint32_t val = recv_pull_uint32(state);
    
                if (recv_verify_checksum(state))
                {
                    send_start_message(state, AVATAR_RPC_DTH_REPLY_OK);
                    RegisterMap_set_register(state, regnum, val);
                    send_end_message(state);
                }
                else
                {
                    send_error_message(state, AVATAR_ERROR_CHECKSUM);
                }

                break;
            }
    //         case AVATAR_RPC_HTD_INSERT_PAGE:
    //         {
    //             uint32_t page_address = recv_pull_uint32(state);
    //             int i;
    //             uint8_t page[AVATAR_PAGE_SIZE];
    //             
    //             for (i = 0; i < AVATAR_PAGE_SIZE; i++)
    //             {
    //                 page[i] = recv_pull_uint8(state);
    //             }
    // 
    //             if (recv_verify_checksum(state))
    //             {
    //             	send_ok_message(state);
    //             	Avatar_memory_insert_page(page_address, page);
    //             }
    //             else
    //             {
    //             	send_error_message(state, AVATAR_ERROR_CHECKSUM);
    //             }
    //             break;
    //         }
    //         case AVATAR_RPC_HTD_EXTRACT_PAGE:
    //         {
    //             uint32_t page_address = recv_pull_uint32();
    //             int i;
    //             int err;
    //             uint8_t page[AVATAR_PAGE_SIZE];
    //             
    //             if (recv_verify_checksum())
    //             {
    //                 err = Avatar_memory_extract_page(page_address, page);
    //                 if (err)
    //                 {
    //                 	send_error_message(err);
    //                 }
    //                 else
    //                 {
    //                     send_start_message(AVATAR_RPC_DTH_REPLY_EXTRACT_PAGE);
    //                     for (i = 0; i < AVATAR_PAGE_SIZE; i++)
    //                     {
    //                         send_push_uint8(page[i]);
    //                     }
    //                     send_end_message();
    //                 }
    //             }
    //             else
    //             {
    //             	send_error_message(AVATAR_ERROR_CHECKSUM);
    //             }
    //             break;
    //         }
    //         case AVATAR_RPC_HTD_UNMAP_PAGE:
    //         {
    //             uint32_t page_address = recv_pull_uint32();
    //             int err;
    //             
    //             if (recv_verify_checksum())
    //             {
    //                 err = Avatar_memory_unmap_page(page_address);
    //                 if (err)
    //                 {
    //                 	send_error_message(err);
    //                 }
    //                 else
    //                 {
    //                 	send_ok_message();
    //                 }
    //             }
    //             else
    //             {
    //             	send_error_message(AVATAR_ERROR_CHECKSUM);
    //             }
    //             break;
    //         }
    //         case AVATAR_RPC_HTD_SET_EXCEPTION_CONFIG:
    //         {
    //         	uint32_t config = recv_pull_uint32();
    //         	uint16_t squelch_irq_limit = recv_pull_uint16();
    //         	uint16_t squelch_fiq_limit = recv_pull_uint16();
    //         	uint32_t exception_vectors[8];
    //         	int i;
    //         	int err;
    // 
    //         	for (i = 0; i < 8; i++)
    //         	{
    //         		exception_vectors[i] = recv_pull_uint32();
    //         	}
    // 
    //         	if (recv_verify_checksum())
    //         	{
    //         		err = Avatar_exception_set_configuration(config, squelch_irq_limit, squelch_fiq_limit, exception_vectors);
    //         		if (err)
    //         			send_error_message(err);
    //         		else
    //         			send_ok_message();
    //         	}
    //         	else
    //         		send_error_message(AVATAR_ERROR_CHECKSUM);
    //             break;
    //         }
    //         case AVATAR_RPC_HTD_CLEAR_EXCEPTION:
    //         {
    //         	uint8_t exception_num = recv_pull_uint8();
    // 
    //         	if (recv_verify_checksum())
    //         	{
    //         		Avatar_exception_clear(exception_num);
    //         		send_ok_message();
    //         	}
    //         	else
    //         		send_error_message(AVATAR_ERROR_CHECKSUM);
    //         	break;
    //         }
    //         case AVATAR_RPC_HTD_ADD_EMULATED_INSTRUCTION:
    //         {
    //             uint32_t instr_address = recv_pull_uint32();
    //             uint8_t code_size = recv_pull_uint8();
    //             int err;
    //             int i;
    //             uint8_t code[AVATAR_MAX_MINIVM_CODE_SIZE];
    //             
    //             if (code_size > AVATAR_MAX_MINIVM_CODE_SIZE)
    //             {
    //                 for (i = 0; i < code_size; i++)
    //                 {
    //                     recv_pull_uint8();
    //                 }
    //                 
    //                 recv_verify_checksum();
    //                 send_error_message(AVATAR_ERROR_OUT_OF_BOUNDS);
    //             }
    //             else
    //             {
    //             	for (i = 0; i < code_size; i++)
    //             	{
    //             		code[i] = recv_pull_uint8();
    //             	}
    //             	if (recv_verify_checksum())
    //             	{
    //             		err = Avatar_minivm_add_emulated_instruction(instr_address, code, code_size);
    //             		if (err)
    //             		{
    //             			send_error_message(AVATAR_RPC_DTH_REPLY_ERROR);
    //             		}
    //             		else
    //             		{
    //             			send_ok_message();
    //             		}
    //             	}
    //             	else
    //             	{
    //             		send_error_message(AVATAR_ERROR_CHECKSUM);
    //             	}
    // 
    //             }
    //             break;
    //         }
    //         case AVATAR_RPC_HTD_CLEAR_EMULATED_INSTRUCTIONS:
    //         {
    //         	if (recv_verify_checksum())
    //         	{
    //         		Avatar_minivm_clear_instructions();
    //         		send_ok_message();
    //         	}
    //         	else
    //         	{
    //         		send_error_message(AVATAR_ERROR_CHECKSUM);
    //         	}
    //         	break;
    //         }
    //         case AVATAR_RPC_HTD_SET_MEMORY_MAP:
    //         {
    //         	uint8_t num_entries = recv_pull_uint8();
    //         	int i;
    //         	int err;
    //         	AvatarMemoryMapEntry memory_map[AVATAR_MAX_MEMORY_MAP_ENTRIES];
    // 
    //         	if (num_entries > AVATAR_MAX_MEMORY_MAP_ENTRIES)
    //         	{
    //         		for(i = 0; i < num_entries * 3; i++)
    //         		{
    //         			recv_pull_uint32();
    //         		}
    //         		recv_verify_checksum();
    //         		send_error_message(AVATAR_ERROR_OUT_OF_BOUNDS);
    //         	}
    //         	else
    //         	{
    //         		for (i = 0; i < num_entries; i++)
    //         		{
    //         			memory_map[i].start = recv_pull_uint32();
    //         			memory_map[i].end = recv_pull_uint32();
    //         			memory_map[i].flags = recv_pull_uint32();
    //         		}
    // 
    //         		if (recv_verify_checksum())
    //         		{
    //         			err = Avatar_memory_set_memory_map(memory_map, num_entries);
    //         			if (err)
    //         				send_error_message(err);
    //         			else
    //         				send_ok_message();
    //         		}
    //         		else
    //         			send_error_message(AVATAR_ERROR_CHECKSUM);
    //         	}
    //         	break;
    //         }
            case AVATAR_RPC_HTD_RESUME_VM:
            {
                if (recv_verify_checksum(state))
                {
                    while (recv_byte_unescape() != SERIAL_BREAK);
                    /* No ok here, response is state message that is sent when VM stops */
                    return;
//                    Avatar_vm_resume(); /* Does not return */
                }
                else
                    send_error_message(state, AVATAR_ERROR_CHECKSUM);
                break;
            }
            case AVATAR_RPC_HTD_QUERY_STATE:
            {
                if (recv_verify_checksum(state))
                {
                    send_start_message(state, AVATAR_RPC_DTH_REPLY_STATE);
                    send_push_uint8(state, state->signal);
                    send_end_message(state);
                }
                else
                {
                    send_error_message(state, AVATAR_ERROR_CHECKSUM);
                }
                break;
            }
    //         case AVATAR_RPC_HTD_GET_DIRTY_PAGES:
    //         {
    //             uint32_t page_addresses[10];
    //             uint8_t num = 10;
    //             uint8_t i;
    // 
    //             if (recv_verify_checksum())
    //             {
    //                 Avatar_memory_get_dirty_pages(page_addresses, &num);
    //                 send_start_message(AVATAR_RPC_DTH_REPLY_GET_DIRTY_PAGES);
    //                 send_push_uint8(num);
    //                 for(i = 0; i < num; i++)
    //                 {
    //                     send_push_uint32(page_addresses[i]);
    //                 }
    //                 send_end_message();
    //             }
    //             else
    //             {
    //                 send_error_message(AVATAR_ERROR_CHECKSUM);
    //             }
    //             break;
    //         }
    //         case AVATAR_RPC_HTD_CODELET_EXECUTE:
    //         {
    //             uint32_t codelet_address = recv_pull_uint32();
    // 
    //             if (recv_verify_checksum())
    //             {
    //                 codelet_execute((void *) codelet_address);
    //                 send_start_message(AVATAR_RPC_DTH_REPLY_CODELET_EXECUTION_FINISHED);
    //                 send_end_message();
    //             }
    //             else
    //             {
    //                 send_error_message(AVATAR_ERROR_CHECKSUM);
    //             }
    //             break;
    //         }
        }

        while (recv_byte_unescape() != SERIAL_BREAK);
    }
}
