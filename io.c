#include "thos.h"
#include "hw.h"

int thos_setup(void)
{
	regs[REG_AHBCLKCTRL] |= REG_AHBCLKCTRL_CT32B1;

	/* enable timer 1, and count at HZ Hz (currently 100) */
	regs[REG_TMR32B1TCR] = 1;
	regs[REG_TMR32B1PR] = (THOS_QUARTZ / HZ) - 1;
	return 0;
}

void putc(int c)
{
	if (c == '\n')
		putc('\r');
	while ( !(regs[REG_U0LSR] & REG_U0LSR_THRE) )
		;
	regs[REG_U0THR] = c;
}

void puts(char *s)
{
	while (*s)
		putc (*s++);
}
