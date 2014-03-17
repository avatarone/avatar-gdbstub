#ifndef _STARTUP_INFORAD_K0_H
#define _STARTUP_INFORAD_K0_H

__attribute__((__naked__)) void _start()  {
          /* TODO: Put startup code here, e.g., stack initialization,
             and then call main. */
    __asm__(
        "ldr sp, =0x60017ffc    \n"
	"bl main                \n"
	"b .                    \n");
}

#endif /* _STARTUP_INFORAD_K0_H */


