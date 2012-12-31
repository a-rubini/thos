#include <stdint.h>

extern volatile uint8_t regs[];

#define THOS_QUARTZ		(16UL * 1000 * 1000)
#define HZ			(THOS_QUARTZ / 256 / 256) /* 244 (+.140625) */

/* uart */
#define REG_UBBRL	0x29
#define REG_UCSRB	0x2a
#define REG_UCSRB_TXEN		0x08
#define REG_UCSRB_UCSZ2		0x04
#define REG_UCSRA	0x2b
#define REG_UCSRA_UDRE		0x20
#define REG_UCSRA_TXC		0x40
#define REG_UCSRA_U2X		0x02
#define REG_UDR		0x2c
#define REG_UCSRC	0x40
#define REG_UCSRC_URSEL		0x80	/* 1 for SR C, 0 for BBRL */
#define REG_UCSRC_UMSEL		0x40	/* 1 for sync, 0 for async */
#define REG_UCSRC_UCSZ1		0x04
#define REG_UCSRC_UCSZ0		0x02

/* timer 0 */
#define REG_TCNT	0x52
#define REG_TCCR0	0x53
#define REG_TCCR0_P256		0x04
#define REG_TIMSK	0x59
#define REG_TIMSK_TOIE0		0x01
