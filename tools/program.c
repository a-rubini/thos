/*
 * Trivial program to write  LPC RAM and go to it 
 * Alessandro Rubini, 2007, GNU GPL2 or later
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/select.h>
#include "lpclib.h"

/* error message */
static void __attribute__((noreturn)) errorexit(char **argv, char *reason)
{
	fprintf(stderr, "%s: %s\n", argv[0], reason);
	fprintf(stderr, "%s: Usage: \"%s <file>\n", argv[0], argv[0]);
	fprintf(stderr, "    $LPC_PORT is the serial port (default %s)\n",
		LPC_PORT);
	fprintf(stderr, "    $LPC_CLK is the clock speed in kHz "
		"(default: %i)\n", LPC_CLK);
	fprintf(stderr, "    $LPC_VERBOSE forces verbose mode (default on)\n");
	fprintf(stderr, "    $LPC_QUIET disables verbose mode\n");
	exit(1);
}

/*
 * Main program
 */
#define V(format, ...) if (verbose) fprintf(stderr, format, ## __VA_ARGS__)
#define MAXSIZE (64*1024)

int main(int argc, char **argv)
{
	static unsigned char filebuf[MAXSIZE];
	unsigned char *ptr; /* binary */
	char s[128]; /* ascii */
	char *port, *reply;
	int fd, i, size, nline, clk, verbose;
	unsigned long check;
	struct lpc_dev *dev;
	FILE *f;

	if (argc != 2) errorexit(argv, "Wrong number of arguments");
	if (!(f = fopen(argv[1], "r"))) {
		sprintf(s, "%s: %s", argv[1], strerror(errno));
		errorexit(argv, s);
	}

	/* get arguments from environment */
	port = getenv("LPC_PORT");
	if (!port)
		port = LPC_PORT;
	clk = LPC_CLK;
	if (getenv("LPC_CLK"))
		clk = atoi(getenv("LPC_CLK"));
	verbose = 1;
	if (getenv("LPC_QUIET"))
		verbose = 0;
	if (getenv("LPC_VERBOSE"))
		verbose = 1;

	/* open serial port and make it raw */
	V("Opening serial port %s\n", port);
	if( (fd=open(port, LPC_OPEN_FLAGS)) < 0){
		fprintf(stderr, "%s: %s: %s\n", argv[0], port, strerror(errno));
		exit(1);
	}
	lpc_fd_configure(fd);

	/* reset, force programming, exit reset */
	V("Forcing boot loader mode\n");
	lpc_fd_forcebootloader(fd);

	/* synchronize (and remove echo) */
	fprintf(stderr, "Syncronizing... ");
	if (lpc_fd_sync(fd, clk) != 0) {
		fprintf(stderr, "failed!\n");
		exit(1);
	}
	fprintf(stderr, "done\n");

	/* Identify part */
	V("Identifying... ");
	i = lpc_fd_identify(fd);
	if (i == -1) {V("failure\n"); exit(1);}
	V("done\n");

	/* Report part to stderr */
	fprintf(stderr, "part number: %x\n", i);
	for (dev = lpc_devs; dev->id && dev->id != i; dev++)
		/* nothing */;
	if (!dev->id) {
		fprintf(stderr, "unknown part, using 2104 (minimal) values\n");
		dev = lpc_devs;
	} else {
		fprintf(stderr, "LPC%i, %ikB Flash, %ikB RAM\n", dev->name,
			dev->rom, dev->ram);
	}

	/* read file and check size */
	size = fread(filebuf, 1, MAXSIZE, f);
	size = (size/900+1) * 900; /* hack: ensure it's a multiple of 20 */
	if (size > dev->ram * 1024 - /* our offset into ram */ 1024)
	errorexit(argv, "file size doesn't fit RAM\n");
	V("size is %i\n", size);

	/* Write data to RAM */
	sprintf(s, "W %lu %i\r\n", dev->type->ram_addr, size);
	V("%s", s);
	reply = lpc_write_c(fd, s, 2); /* echo and error code */
	V("%s", reply);
	check = 0;
	for (ptr = filebuf, nline = 0, i = 0; i < size;) {
		int j;
		lpc_uuencode(ptr, 45, s);
		V(".");
		for (j=0; j<45; j++)
			check += ptr[j];
		strcat(s, "\r\n");
		lpc_write_c(fd, s, 1);
		ptr += 45; i += 45;
		nline++;
		if (nline && nline%20 == 0) {
			/* print checksum read OK */
			sprintf(s, "%li\r\n", check); check = 0;
			reply = lpc_write_c(fd, s, 2);
			V("%s", reply); /* OK Hopefully */
		}
	}

	/* hopefully, everything worked. Now go to the address */
	sprintf(s, "U 23130\r\n");
	lpc_write_c(fd, s, 2);
	sprintf(s, "G %lu %c\r\n", dev->type->ram_addr, dev->type->mode);
	lpc_write_c(fd, s, 2);
	while(1) {
		fd_set fds; struct timeval tv;

		if (lpc_fd_gets(fd, s, 80) >=0) /* data */
			V("%s", s);
		/* else read stdin, which is line-buffered */
		FD_ZERO(&fds);	FD_SET(0, &fds);
		tv.tv_sec = tv.tv_usec = 0;
		if (select(1, &fds, NULL, NULL, &tv) > 0) {
			if (fgets(s, 80, stdin) == NULL)
				exit(0);
			lpc_write_c(fd, s, 0);
		}
	}
	return 0;
}
