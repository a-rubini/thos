#include <stdint.h>

extern volatile uint32_t regs[];

#define THOS_QUARTZ		(14745600 / 4) /* Periph clock = quartz / 4 */
#define HZ			100


/* uart */
#define REG_U0THR		(0xe000c000 / 4)
#define REG_U0LSR		(0xe000c014 / 4)
#define REG_U0LSR_THRE		0x20

/* timer 0 */
#define REG_T0PR		(0xe000400c / 4)
#define REG_T0TCR		(0xe0004004 / 4)

/* gpio and pin select */
#define REG_IOPIN		(0xe0028000 / 4)
#define REG_IOSET		(0xe0028004 / 4)
#define REG_IODIR		(0xe0028008 / 4)
#define REG_IOCLR		(0xe002800c / 4)
#define REG_PINSEL0		(0xe002c000 / 4)
#define REG_PINSEL1		(0xe002c004 / 4)

/* pwm */
#define REG_PWMTCR		(0xe0014004 / 4)
#define REG_PWMTC		(0xe0014008 / 4)
#define REG_PWMPR		(0xe001400c / 4)
#define REG_PWMPC		(0xe0014010 / 4)
#define REG_PWMMR0		(0xe0014018 / 4)
#define REG_PWMMR2		(0xe0014020 / 4)
#define REG_PWMMCR		(0xe0014014 / 4)
#define REG_PWMPCR		(0xe001404c / 4)
#define REG_PWMLER		(0xe0014050 / 4)
