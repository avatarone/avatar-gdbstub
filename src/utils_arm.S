/*
 * Copyright 2013-2014 Jonas Zaddach <zaddach@eurecom.fr>, EURECOM
 *
 * You can redistribute and/or modify this program under the terms of the
 * GNU General Public License version 2 or later.
 */

.global __gnu_thumb1_case_si
.global __gnu_thumb1_case_uqi

.thumb
.align 2

.thumb_func
__gnu_thumb1_case_si:
    push {r0, r1}
    mov r1, lr
    add r1, r1, #2  /* Align to word.  */
    lsr r1, r1, #2
    lsl r0, r0, #2
    lsl r1, r1, #2
    ldr r0, [r1, r0]
    add r0, r0, r1
    mov lr, r0
    pop {r0, r1}
    mov pc, lr

.thumb_func
__gnu_thumb1_case_uqi:
    push    {r1}
    mov r1, lr
    lsr    r1, r1, #1
    lsl    r1, r1, #1
    ldrb    r1, [r1, r0]
    lsl    r1, r1, #1
    add lr, lr, r1
    pop {r1}
    bx  lr
