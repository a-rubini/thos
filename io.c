#include "thos.h"
#include "hw.h"

int thos_setup(void)
{ return 0; }

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
