/*
 * Library of generic code for LPC access
 * Alessandro Rubini, 2007, GNU GPL2 or later
 */
#include <stdio.h>
#include <termios.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#include "lpclib.h"

/*
 * device table
 */
struct lpc_dev lpc_devs[] = {
	{0xFFF0FF12, 2104, 120, 16},
	{0xFFF0FF22, 2105, 120, 32},
	{0xFFF0FF32, 2106, 120, 64},
	/* the table is incomplete */
	{0x2c42502b, 1311,   8,  4},
	{0x2c40102b, 1313,  32,  8},
	{0x3d01402b, 1342,  16,  4},
	{0x3d00002b, 1343,  32,  8},
	{0,}
};


/*
 * TTY section
 */
#define LPC_TTY_IFLAG    (IGNBRK  |IGNPAR)
#define LPC_TTY_OFLAG    0
#define LPC_TTY_CFLAG    (CS8 | CREAD | CLOCAL)
#define LPC_TTY_LFLAG    0
#define LPC_TTY_SPEED    B115200

/* tty fixup (after O_NDELAY open) */
int lpc_fd_configure(int fd)
{
	struct termios newTermIo;

	fcntl(fd, F_SETFL, 0); /* clear ndelay */

	memset(&newTermIo, 0, sizeof(struct termios));

	newTermIo.c_iflag = LPC_TTY_IFLAG;
	newTermIo.c_oflag = LPC_TTY_OFLAG;
	newTermIo.c_cflag = LPC_TTY_CFLAG;
	newTermIo.c_lflag = LPC_TTY_LFLAG;
	newTermIo.c_cc[VMIN] = 1;
	newTermIo.c_cc[VTIME] = 0;

	if (cfsetispeed(&newTermIo, LPC_TTY_SPEED) < 0)
		return -1;
	if (tcsetattr(fd, TCSANOW, &newTermIo) < 0)
		return -1;
	tcflush(fd, TCIOFLUSH);
	return 0;
}

/*
 * Primitives for serial management
 */
void lpc_fd_forcebootloader(int fd)
{
	unsigned int bits;

	ioctl(fd,TIOCMGET,&bits);
	bits |= TIOCM_RTS | TIOCM_DTR; /* reset and p0.14 */
	ioctl(fd,TIOCMSET,&bits);
	usleep(100000); /* 0.1s */
	bits &= ~TIOCM_DTR; /* remove reset */
	ioctl(fd,TIOCMSET,&bits);
	usleep(100000); /* 0.1s */
}

int lpc_serial_timeout = 200*1000; /* usec */

int lpc_fd_gets(int fd, char *s, int len)
{
	int i, l;
	/* read line, with a timeout */
	for (l=0; l<len; l++) {
		fd_set fds; struct timeval tv;
		FD_ZERO(&fds);	FD_SET(fd, &fds);
		tv.tv_sec = 0; tv.tv_usec = lpc_serial_timeout;
		i = select(fd+1, &fds, NULL, NULL, &tv);
		if (i <= 0) {  /* timeout or error */
			if (l) {s[l] = '\0'; return l;}
			else return -1;
		}
		i = read(fd, s+l, 1);
		if (i < 0) return -1;
		if (!i) break;
		/* LPC210x uses \r\n, but LPC3134 uses \r alone: ignore \n */
		if (s[l]=='\n') {l--; continue;}
		/* but return \n to the called, in the best C tradition */
		if (s[l]=='\r') {s[l++] = '\n'; break;}
	}
	s[l] = '\0';
	return l;
}

int lpc_fd_sync(int fd, int clk)
{
	char s[32];
	char *reply;
	int i;

	for (i = 0; i < 10; i++) {
		strcpy(s, "?\r\n"); /* return string is longer */
		reply = lpc_write_c(fd, "?\r\n", 1);
		if (!reply)
			continue;
		if (!strncmp(reply, "Synchronized", 12))
			break;
	}
	if (i == 10) return -1;
	if ( (reply = lpc_write_c(fd, "Synchronized\r\n", 2)) == NULL)
		return -1;
	if (strncmp(reply, "OK", 2))
		return -1;
	sprintf(s, "%i\r\n", clk);
	if ( (reply = lpc_write_c(fd, s, 2))  == NULL)
		return -1;
	if (strncmp(reply, "OK", 2))
		return -1;
	return 0;
}


unsigned long lpc_fd_identify(int fd)
{
	unsigned long i;
	char *reply;

	if ( (reply = lpc_write_c(fd, "J\r\n", 2)) == NULL)
		return -1;
	if (!sscanf(reply, "%lu", &i) || i != 0)
		return -1; /* error code */
	if (lpc_fd_gets(fd, reply, 32) < 0)
		return -1;
	if (!sscanf(reply, "%lu", &i))
		return -1;
	return i;
}

/*
 * Write a constant string, read back N of them
 */
char *lpc_write_c(int fd, char *string, int nrd)
{
	static char s[80];
	int i = strlen(string);

	if (0)
		fprintf(stderr, "%i: %s", nrd, string);

	strcpy(s, string);
	if (write(fd, string, i) != i)
		return NULL;
	/* read back can be longer, blame the caller if needed */
	for (i = 0; i < nrd; i++)
		if (lpc_fd_gets(fd, s, sizeof(s)) < 0)
			return NULL;
	return s;
}

/*
 * uuencode and uudecode
 */
void lpc_uuencode(unsigned char *in, int c, char *out)
{
	int i, j; unsigned l;

	*out = ' ' + c;
	for (i = 0, j = 1; i < c; i += 3) {
		l = in[i]<<16 | in[i+1]<<8 | in[i+2];
		/* this uses '`' instead of ' '  */
		out[j++] = ' ' + (((l>>18)-1) & 0x3f) + 1;
		out[j++] = ' ' + (((l>>12)-1) & 0x3f) + 1;
		out[j++] = ' ' + (((l>> 6)-1) & 0x3f) + 1;
		out[j++] = ' ' + (((l>> 0)-1) & 0x3f) + 1;
	}
	out[j] = '\0';
	return;
}

int lpc_uudecode(char *in, unsigned char *out)
{
	int i, j; unsigned l;
	int c = in[0]-' ';

	for (i = 0, j = 1; i < c; j += 4) {
		l = (((in[j]-' ')&0x3f) << 18)
			| (((in[j+1]-' ')&0x3f) << 12)
			| (((in[j+2]-' ')&0x3f) << 6)
			| ((in[j+3]-' ')&0x3f);
		out[i++] = l>>16;
		out[i++] = (l>>8) & 0xff;
		out[i++] = l&0xff;
	}
	return c;
}

/*
 * Map user flash (or vectors are still ROM ones) at 0x40048000
 */
int lpc_map_user_flash(int fd)
{
	char s[32];
	char *reply;

	/* The "W" command only accepts RAM addresses. So send code:
	 *  0:   f248 0300       movw    r3, #32768      ; 0x8000
	 *  4:   f2c4 0304       movt    r3, #16388      ; 0x4004
	 *  8:   2202            movs    r2, #2
	 *  a:   601a            str     r2, [r3, #0]
	 *  c:   4770            bx      lr
	 *  e:   bf00            nop
	 */
	static unsigned char code[] = {
		0x48, 0xf2, 0x00, 0x03,
		0xc4, 0xf2, 0x04, 0x03,
		0x02, 0x22, 0x1a, 0x60,
		0x70, 0x47, 0x00, 0xbf
	};
	int i, check = 0;

	sprintf(s, "W %u %i\r\n", 0x10000400, sizeof(code));
	reply = lpc_write_c(fd, s, 2); /* echo and error code */
	lpc_uuencode(code, sizeof(code), s);
	strcat(s, "\r\n");
	lpc_write_c(fd, s, 1);
	for (i = 0; i < sizeof(code); i++)
		check += code[i];
	sprintf(s, "%i\r\n", check);
	reply = lpc_write_c(fd, s, 2);
	fprintf(stderr, "Sent memmap code: %s", reply);
	/* Then unlock and go. Oh boring... */
	sprintf(s, "U 23130\r\n");
	lpc_write_c(fd, s, 2);
	sprintf(s, "G %i T\r\n", 0x10000400);
	lpc_write_c(fd, s, 2);
	return 0;
}
