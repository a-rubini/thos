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

#define PWM_FREQ (THOS_QUARTZ / 4) /* we make a 4-cycles-long pwm */

struct note table[] = {
	{'f', (PWM_FREQ/F) + 0.5},
	{'g', (PWM_FREQ/G) + 0.5},
	{'a', (PWM_FREQ/A) + 0.5},
	{'b', (PWM_FREQ/B) + 0.5},
	{'c', (PWM_FREQ/C) + 0.5},
	{'d', (PWM_FREQ/D) + 0.5},
	{0, ~0}, /* pause */
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
	/* Turn on power to timer 0 */
	regs[REG_AHBCLKCTRL] |= REG_AHBCLKCTRL_CT32B0;
	/* AF for P0_1: use alternate function 2 */
	regs[0x40044010 / 4] = 0x82;

	regs[REG_TMR32B0TCR] = 1;		/* enable */
	regs[REG_TMR32B0PR] = ~0;		/* prescaler: looong */
	regs[REG_TMR32B0MR3] = 4;		/* match reg 3 */
	regs[REG_TMR32B0MCR] = 0x400;		/* reset on MR3 */
	regs[REG_TMR32B0MR2] = 2;		/* match reg 2 */
	regs[REG_TMR32B0PWMC] = 4;		/* use MR2 for pwm on P0_1 */

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

	/* activate it by writing the prescaler and resetting the timer */
	regs[REG_TMR32B0PR] = n->period;
	regs[REG_TMR32B0TCR] = 3;
	regs[REG_TMR32B0TCR] = 1;

	return s + 1;

}

static struct thos_task __task t_pwm = {
	.name = "pwm", .period = HZ / 10,
	.init = pwm_init, .job = pwm,
	.release = 5
};
