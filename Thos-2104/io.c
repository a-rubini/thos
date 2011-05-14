#include "thos.h"
#include "hw.h"

int thos_setup(void)
{
	/* enable timer 0, and count at HZ Hz (currently 100) */
	regs[REG_T0TCR] = 1;
	regs[REG_T0PR] = (THOS_QUARTZ / HZ) -1;
	regs[REG_T0TCR] = 3;
	regs[REG_T0TCR] = 1;
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
