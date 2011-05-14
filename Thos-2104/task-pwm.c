#include "thos.h"
#include "hw.h"

#define HALF 1.05946309435929526455 /* exp(2, 1/12) */
#define TONE (HALF*HALF)

#define F    (G/TONE)
#define G    (A/TONE)
#define A    440.0
#define B    (A*HALF) /* moll */
#define C    (B*TONE)
#define D    (C*TONE)

struct note {
	char name;
	int period;
};

#define PWM_FREQ THOS_QUARTZ /* Input frequency to all peripherals */

struct note table[] = {
	{'f', (PWM_FREQ/F) + 0.5},
	{'g', (PWM_FREQ/G) + 0.5},
	{'a', (PWM_FREQ/A) + 0.5},
	{'b', (PWM_FREQ/B) + 0.5},
	{'c', (PWM_FREQ/C) + 0.5},
	{'d', (PWM_FREQ/D) + 0.5},
	{0, 0}, /* pause */
};

#if 1
static char tune[] =
	"f f f a c c ccc d d d b ccc aaa "
	"f f f f a a a a g g g g fffff   "
	"                                ";
#else
static char tune[] =
	"f a a a gfccc "
	"cba a a gfccc "
	"cba bcb a g gggg    ";
#endif


static int pwm_init(void *unused)
{
	/* Change AF for gpio 8 */
	uint32_t val = regs[REG_PINSEL0];
	val &= ~0xc000;
	val |=  0x8000;
	regs[REG_PINSEL0] = val;

	/* configure the PWM device */
	regs[REG_PWMTCR] = 2;
	regs[REG_PWMMCR] = 0;
	regs[REG_PWMPCR] = 0x400;
	regs[REG_PWMTCR] = 9;

	return 0;
}

static void *pwm(void *arg)
{
	char *s = arg;
	struct note *n = table;

	if (!s || !*s) s = tune; /* lazy */

	/* look for freq */
	for (n = table; n->name && n->name != *s; n++)
		;

	/* activate it by making a short pulse at end-of-period */
	regs[REG_PWMPR] = 0;
	regs[REG_PWMMR0] = n->period;
	regs[REG_PWMMR2] = n->period - 0x100;
	regs[REG_PWMLER] = 5;
	regs[REG_PWMTC] = 0;

	return s + 1;

}

static struct thos_task __task t_pwm = {
	.name = "pwm", .period = HZ / 10,
	.init = pwm_init, .job = pwm,
	.release = 5
};
