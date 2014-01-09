/*
 * Copyright 2013-2014 Lucian Cojocar <lucian.cojocar@vu.nl>, VU
 *
 * You can redistribute and/or modify this program under the terms of the
 * GNU General Public License version 2 or later.
 */

OUTPUT_FORMAT("elf32-bigarm", "elf32-bigarm", "elf32-bigarm")
OUTPUT_ARCH(arm)
ENTRY(_start)
SECTIONS
{
    . = 0x16000 ;
    _gdb_stub_start = .;
    .text         : { *(.header) *(.start) *(.text) }
    .data         : { *(.data) }
    .bss          : { *(.bss) }
    _gdb_stub_end = .;
}
