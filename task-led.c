#include "thos.h"
#include "hw.h"

static int led_init(void *unused)
{
	regs[REG_AHBCLKCTRL] |= REG_AHBCLKCTRL_GPIO;
	regs[REG_GPIO3DIR] |= 0xf;
	return 0;
}

static void *led(void *arg)
{
	int value, state = (int)arg;

	if (state > 4)
		state = 0;
	switch (state) {
	case 4:
		value = 0; /* all off */
		break;
	default:
		value = 1 << state;
		break;
	}
	regs[REG_GPIO3DAT] =  0xf & ~value;
	return (void *)(state + 1);
}

static struct thos_task __task t_led = {
	.name = "leds", .period = HZ / 5,
	.init = led_init, .job = led,
	.release = 10
};
