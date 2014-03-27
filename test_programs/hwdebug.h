#ifndef _HWDEBUG_H
#define _HWDEBUG_H

static unsigned max_context_id_breakpoints;
static unsigned max_breakpoints;
static unsigned max_watchpoints;
static unsigned debug_version;

#define hwdebug_enable() cp14_set_dscr(1 << 15)
#define hwdebug_disable() cp14_set_dscr(0)

static int hwdebug_init(void)
{
	unsigned i;
	uint32_t didr = cp14_get_didr();
	
	debug_version = (didr >> 16) & 0xf;
	max_context_id_breakpoints = ((didr >> 20) & 0xf) + 1;
	max_breakpoints = ((didr >> 24) & 0xf) + 1;
	max_watchpoints = ((didr >> 28) & 0xf) + 1;
	
	if (max_breakpoints != 0)  {
		max_breakpoints += 1;
	}
	
	cp14_set_vcr(0); //Do not catch any exception vectord
	for (i = 0; i < max_breakpoints; i++)  {
		cp14_set_bcr(0);
	}
	for (i = 0; i < max_watchpoints; i++)  {
		cp14_set_wcr(0);
	}
}

static int hwdebug_set_breakpoint(uint32_t address, )

#endif /* _HWDEBUG_H */