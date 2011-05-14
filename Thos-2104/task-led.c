#include "thos.h"
#include "hw.h"

static int led_init(void *unused)
{
	regs[REG_IODIR] |= 7 << 19; /* gpio 19, 20, 21 */
	return 0;
}

static void *led(void *arg)
{
	int i = (int)arg & 7;

	regs[REG_IOSET] = i << 19;
	regs[REG_IOCLR] = (i^7) << 19;
	return (void *)++i;
}

static struct thos_task __task t_led = {
	.name = "leds", .period = HZ / 2,
	.init = led_init, .job = led,
	.release = 10
};
