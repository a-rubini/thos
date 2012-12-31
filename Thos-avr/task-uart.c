#include "thos.h"
#include "hw.h"

static void *uart_out(void *arg)
{
	char *s = arg;
	puts(s);
	return arg;
}

static struct thos_task __task t_quarter = {
	.name = "quarter", .period = HZ/4,
	.job = uart_out, .arg = "."
};

static struct thos_task __task t_second = {
	.name = "second", .period = HZ,
	.job = uart_out, .arg = "S",
	.release = 1,
};

static struct thos_task __task t_10second = {
	.name = "10second", .period = 10 * HZ,
	.job = uart_out, .arg = "\n",
	.release = 2,
};

static struct thos_task __task t_minute = {
	.name = "minute", .period = 60 * HZ,
	.job = uart_out, .arg = "minute!\n",
	.release = 3,
};
