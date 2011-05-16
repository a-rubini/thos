/*
 * Library of generic code for LPC access
 * Alessandro Rubini, 2007, GNU GPL2 or later
 */
#ifndef __LPCLIB_H__
#define __LPCLIB_H__

#include <unistd.h>
#include <string.h>

/* defaults */
#define LPC_PORT "/dev/ttyUSB0"
#define LPC_CLK 14746

/* This stuff supports both arm7 and cortex-m */
struct lpc_type {
	unsigned long ram_addr;
	unsigned long default_clock;
	int mode; /* 'A' or 'T' */
	int sector_size;
	int checksum_vector;

	/* for "map_user_flash" */
	unsigned long remap_addr;
	unsigned char *code;
	int codesize;
};

/* device table */
struct lpc_dev {
	unsigned long id;
	int name, rom, ram;
	struct lpc_type *type;
};
extern struct lpc_dev lpc_devs[];

/* tty raw */
#define LPC_OPEN_FLAGS   (O_RDWR  | O_NOCTTY | O_NONBLOCK)
extern int lpc_fd_configure(int fd);

/*
 * Primitives for serial management
 */
extern void lpc_fd_forcebootloader(int fd);
extern int lpc_fd_gets(int fd, char *s, int len);
extern int lpc_fd_sync(int fd, int clk);
extern unsigned long lpc_fd_identify(int fd);

/* write constant string and read back N strings (returns last) */
extern char *lpc_write_c(int fd, char *string, int nrd);

/* trim \r\n */
static inline void lpc_trim(char *s)
{
	if (s[strlen(s)-1] == '\n')
		s[strlen(s)-1] = '\0';
	if (s[strlen(s)-1] == '\r')
		s[strlen(s)-1] = '\0';
}

/* uuencode and uudecode */
extern void lpc_uuencode(unsigned char *in, int c, char *out);
extern int lpc_uudecode(char *in, unsigned char *out);

/* map user flash by calling a short program on the target */
int lpc_map_user_flash(int fd, struct lpc_type *type);

#endif /* __LPCLIB_H__ */
