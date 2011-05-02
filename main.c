#include "thos.h"
#include "hw.h"

int thos_main(void)
{
	unsigned long j = jiffies;
	while (1) {
		puts("The might Thos is alive\n");
		j += HZ;
		while (jiffies < j)
			;
	}
}
