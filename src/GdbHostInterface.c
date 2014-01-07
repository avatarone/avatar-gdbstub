/*
 * Copyright 2013-2014 Jonas Zaddach <zaddach@eurecom.fr>, EURECOM
 *
 * You can redistribute and/or modify this program under the terms of the
 * GNU General Public License version 2 or later.
 */

#include "Serial.h"
#include "StubState.h"
#include "Memory.h"
#include "RegisterMap.h"
#include "ArmGdbHelpers.h"
#include "Assert.h"
#include "gdb_utils.h"

static void target_enter_monitor(StubState *state)
{
}

static void target_exit_monitor(StubState *state)
{
}

static void start_packet(StubState *state);
static void put_string(StubState *state, const char * str);
static void end_packet(StubState *state);

static void put_hex_uint8(StubState *state, uint8_t val)
{
    char c = nibble_to_hex_char(val >> 4);
    state->host_interface.output_checksum += c;
    Serial_write_byte(c);
    c = nibble_to_hex_char(val);
    state->host_interface.output_checksum += c;
    Serial_write_byte(c);
}

void put_hex_buffer(StubState *state, const uint8_t * buffer, int len)
{
    int i;
    char c;
    
    for (i = 0; i < len; i++)
    {
        c = nibble_to_hex_char(buffer[i] >> 4);
        state->host_interface.output_checksum += c;
        Serial_write_byte(c);
        c = nibble_to_hex_char(buffer[i]);
        state->host_interface.output_checksum += c;
        Serial_write_byte(c);
    }
}

static void put_buffer(StubState *state, const uint8_t * buffer, int len)
{
    int i;
    
    for (i = 0; i < len; i++)
    {
        state->host_interface.output_checksum += buffer[i];
       Serial_write_byte(buffer[i]);
    }
}

static void put_ok_packet(StubState *state)
{
	start_packet(state);
	put_string(state, "OK");
	end_packet(state);
}

static void put_error_packet(StubState *state, int error)
{
	start_packet(state);
	put_string(state, "E");
	put_hex_uint8(state, error & 0xff);
	end_packet(state);
}

static void put_dummy_bytes(StubState *state, int count)
{
    int i;
    
    for (i = 0; i < count; i++)
        put_hex_uint8(state, 0xFE);
}

void put_string(StubState *state, const char * str)
{
    put_buffer(state, (const uint8_t *) str, strlen(str));
}
    

void start_packet(StubState *state)
{
    state->host_interface.output_checksum = 0;
    Serial_write_byte('$');
}

void end_packet(StubState *state)
{
    Serial_write_byte('#');
    put_hex_uint8(state, state->host_interface.output_checksum);
}

static void put_signal_packet(StubState *state)
{
    start_packet(state);
    put_string(state, "S");
    put_hex_uint8(state, state->signal);
    end_packet(state);
}

static void put_empty_packet(StubState *state)
{
    start_packet(state);
    end_packet(state);
}

void put_hex_uint32(StubState *state, uint32_t val)
{
    put_hex_uint8(state, val >> 24);
    put_hex_uint8(state, val >> 16);
    put_hex_uint8(state, val >> 8);
    put_hex_uint8(state, val);
}

static uint32_t get_hex_uint32(StubState *state, int len)
{
    uint32_t val = 0;
    char c;
    int i;
    
    for (i = 0; i < 2 * len; i++)
    {
        c = Serial_read_byte_blocking();
        if (c == '#')
        {
            state->host_interface.input_packet_finished = 1;
            return val;
        }
              
        state->host_interface.input_checksum += c;
        
        if (hex_char_to_nibble(c) == -1)  
            return val;
   
        val = (val << 4) | hex_char_to_nibble(c);
    }
    
    return val;
} 

static void get_hex_buffer(StubState *state, void * buffer, int len)
{
    int i = 0;
    
    for (i = 0; i < len; i++)
    {
        ((uint8_t *) buffer)[i] = get_hex_uint32(state, 1);
    }
}

static int received_packet_verify(StubState *state)
{
    uint8_t calculated_checksum = state->host_interface.input_checksum;
    uint8_t received_checksum = get_hex_uint32(state, 1);
    
    if (state->host_interface.input_packet_finished && (calculated_checksum == received_checksum))
    {
        Serial_write_byte('+');
        Serial_flush_write();
        return 1;
    }
    else {
        Serial_write_byte('-');
        Serial_flush_write();
        return 0;
    }
}  

static int receive_packet_end(StubState *state)
{
    if (Serial_read_byte_blocking() == '#')
    {
        state->host_interface.input_packet_finished = 1;
        return received_packet_verify(state);
    }
    else
    {
        return 0;
    }
}

static int get_packet_byte(StubState *state)
{
    char c;
    
    if (state->host_interface.input_packet_finished)
        return -1;
    
    c = Serial_read_byte_blocking();
    
    if (c == '#')
    {
        state->host_interface.input_packet_finished = 1;
        return -1;
    }
    else
    {
        state->host_interface.input_checksum += c;
        return c;
    }
}

// static void get_ignore_bytes(StubState *state, int count)
// {
//     int i;
//     
//     for (i = 0; i < count; i++)
//         get_hex_uint32(state, 1);
// }

static int ignore_rest_of_packet(StubState *state)
{
    while (get_packet_byte(state) != -1);
    
    state->host_interface.input_packet_finished = 1;
    return received_packet_verify(state);
}

void HostInterface_init(StubState *state)
{
}
 
 
void HostInterface_communicate(StubState *state)
//void gdb_monitor(int signal, uint32_t * register_file)
{
//    struct GDB gdb = {.signal = signal, .register_file = register_file};
    
//    gdb_init(&state->host_interface);
    
    target_enter_monitor(state);
 
    put_signal_packet(state);
    
    while (1)
    {
        //wait for packet header
        while (Serial_read_byte_blocking() != '$');
        state->host_interface.input_checksum = 0;
        state->host_interface.input_packet_finished = 0;
        
        switch(get_packet_byte(state))
        {
            case 'm':
            {
                uint32_t address = get_hex_uint32(state, 8);
                uint32_t len = get_hex_uint32(state, 8);
                
                if (received_packet_verify(state))
                {
                    do
                    {
                        start_packet(state);
                        
                        if (!Memory_is_valid_address(state, address) || !Memory_is_valid_address(state, address + len - 1))
                        {
                            put_dummy_bytes(state, len);
                        }
                        else
                        {
                            if ((len == 4 && ((address & 3) == 0)) || (len == 2 && ((address & 1) == 0)) || len == 1)
                            {
                                value_t buf = Memory_read_typed(state, address, len);
                                //TODO: Only works on little endian
                                put_hex_buffer(state, (const uint8_t *) &buf, len);
                                
                            }
                            else
                            {
                                uint32_t i;
                                uint8_t buf;
                        
                                for (i = 0; i < len; i++)
                                {
                                    buf = Memory_read_typed(state, address + i, SIZE_CHAR);
                                    put_hex_buffer(state, (const uint8_t *) &buf, 1);
                                }
                            }
                        }
                    
                        end_packet(state);
                    } while (Serial_read_byte_blocking() != '+');
                }
                break;
            }
            case 'M':
            {
                uint32_t address = get_hex_uint32(state, 9);
                uint32_t len = get_hex_uint32(state, 9);
                
                if (!Memory_is_valid_address(state, address) || !Memory_is_valid_address(state, address + len - 1))
                {
                    ignore_rest_of_packet(state);
                    put_error_packet(state, 1);
                }
                else
                {
                    if ((len == 4 && ((address & 0x3) == 0)) || (len == 2 && ((address & 1) == 0)) || len == 1)
                    {
                        uint32_t buf;
                        get_hex_buffer(state, &buf, len);
                        Memory_write_typed(state, address, len, buf);
                    }
                    else 
                    {
                        uint32_t i;
                        uint8_t buf;
                        
                        for (i = 0; i < len; i++)
                        {
                            get_hex_buffer(state, &buf, 1);
                            Memory_write_typed(state, address + i, SIZE_CHAR, buf); 
                        }
                    }
                    
                    receive_packet_end(state);
                    put_ok_packet(state);
                }
                break;
            }
            case 'g':
            {
                if (receive_packet_end(state))
                {
                    int i;
                    register_t val;
                    
                    do
                    {
                        start_packet(state);
                        //TODO: ARM specific
                        for (i = 0; i < 16; i++)
                        {
                            val = RegisterMap_get_register(state, Gdb_map_gdb_register_number_to_stub(state, i));
                            put_hex_buffer(state, (const uint8_t *) &val, 4);
                        }
                        
                        val = RegisterMap_get_register(state, Gdb_map_gdb_register_number_to_stub(state, 25));
                        put_hex_buffer(state, (const uint8_t *) &val, 4);
                        
                        end_packet(state);
                    } while (Serial_read_byte_blocking() != '+');
                }
                break;
            }
            case 'G':
            {
                int i;
                register_t val;
                /* GDB should have requested a register packet ('g') at this point to know how many registers we expect */
                //TODO: ARM specific
                for (i = 0; i < 16; i++)
                {
                    get_hex_buffer(state, (uint8_t *) &val, 4);
                    RegisterMap_set_register(state, Gdb_map_gdb_register_number_to_stub(state, i), val);     
                }
                
                get_hex_buffer(state, (uint8_t *) &val, 4);
                RegisterMap_set_register(state, Gdb_map_gdb_register_number_to_stub(state, 25), val);
                    
                ignore_rest_of_packet(state);
                break;
            }
            case 'c':
            {
                if (receive_packet_end(state))
                {
                    Gdb_continue_execution(state);
                    target_exit_monitor(state);
                    return;
                }
                else {
                    //We do not support 'continue at address'
                    assert(0 && "Continue at address not supported");
                    ignore_rest_of_packet(state);
                    put_signal_packet(state);
                }
            }
            case '?':
            {
                if (receive_packet_end(state))
                    put_signal_packet(state);
                break;
            }
            case 'p':
            {
                uint32_t reg_id = get_hex_uint32(state, 8);
                register_t val;
                
                if (received_packet_verify(state))
                {
                    val = RegisterMap_get_register(state, Gdb_map_gdb_register_number_to_stub(state, reg_id));
                    start_packet(state);
                    //TODO: Register size is ARM specific
                    put_hex_buffer(state, (const uint8_t *) &val, 4);
                    end_packet(state);
                }
                break;
            } 
            case 'P':
            {
                uint32_t reg_id = get_hex_uint32(state, 8);
                register_t val;
                get_hex_buffer(state, (uint8_t *) &val, 4);
                RegisterMap_set_register(state, Gdb_map_gdb_register_number_to_stub(state, reg_id), val);
                receive_packet_end(state);
                put_ok_packet(state);
                break;
            } 
            default:
            {
                if (ignore_rest_of_packet(state))
                    put_empty_packet(state);
                break;
            }
                
        }
    }
}
    
    
    
    
    
    
