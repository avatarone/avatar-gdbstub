/*
 * Copyright 2013-2014 Jonas Zaddach <zaddach@eurecom.fr>, EURECOM
 *
 * You can redistribute and/or modify this program under the terms of the
 * GNU General Public License version 2 or later.
 */

OUTPUT_FORMAT("elf32-littlearm", "elf32-littlearm", "elf32-littlearm")
OUTPUT_ARCH(arm)
ENTRY(_start)
SECTIONS
{
    _gdb_stub_start = .;
    .text         : { *(.exception_table) *(.init) *(.text) }
    .data         : { *(.data) }
    .bss          : { *(.bss) }
    
/*
    .debug_info   : { *(.debug_info) } 
    .rel.debug_info : { *(.rel.debug_info) }
    .debug_abbrev : { *(.debug_abbrev) }
    .debug_loc    : { *(.debug_loc) }
    .debug_aranges : { *(.debug_aranges) }
    .rel.debug_arange : { *(.rel.debug_arange) }
    .debug_line   : { *(.debug_line) }
    .rel.debug_line : { *(.rel.debug_line) }
    .debug_str    : { *(.debug_str) }
    .comment      : { *(.comment) }
    .ARM.attributes : { *(.ARM.attributes) }
    .debug_frame  : { *(.debug_frame) }
    .rel.debug_frame : { *(.rel.debug_frame) }
    .shstrtab : { *(.shstrtab) }
    .symtab : { *(.symtab) }	*/
    _gdb_stub_end = .;
}
