/*
 * Trivial program to dump the LPC flash 
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
static void errorexit(char **argv, char *reason)
{
	fprintf(stderr, "%s: %s\n", argv[0], reason);
	fprintf(stderr, "%s: Usage: \"%s <start> <len>\n", argv[0], argv[0]);
	fprintf(stderr, "    $LPC_PORT is the serial port (default %s)\n",
		LPC_PORT);
	fprintf(stderr, "    $LPC_CLK is the clock speed in kHz "
		"(default: %i)\n", LPC_CLK);
	fprintf(stderr, "    $LPC_VERBOSE forces verbose mode\n");
	fprintf(stderr, "    $LPC_BINARY forces binary data to stdout\n");
	exit(1);
}

/*
 * Main program
 */
#define V(format, ...) if (verbose) fprintf(stderr, format, ## __VA_ARGS__)

int main(int argc, char **argv)
{
	char *port, *reply;
	int fd, i, nline, clk, verbose, binary;
	struct lpc_dev *dev;
	char s[128]; /* string */
	unsigned char t[128]; /* binary */
	unsigned long start, len;

	if (argc != 3) errorexit(argv, "Wrong number of arguments");
	if (strlen(argv[1]) > 120 || sscanf(argv[1], "%lu%s", &start, t) != 1)
		errorexit(argv, "Argument 1 (start) is not a number");
	if (strlen(argv[2]) > 120 || sscanf(argv[2], "%li%s", &len, t) != 1)
		errorexit(argv, "Argument 2 (len) is not a number");
	if (start & 3 || len & 3)
		errorexit(argv, "Start and len must be multiples of 4");

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
	binary = 0;
	if (getenv("LPC_BINARY"))
		binary = 1;

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

	/* synchronize */
	V("Syncronizing... ");
	i = lpc_fd_sync(fd, clk);
	if (!i) {V("done\n");}
	else {V("failure\n"); exit(1);}

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

	/* Force user flash on, or we'll read back ROM vectors */
	lpc_map_user_flash(fd, dev->type);

	/* Read data to stdout */
	sprintf(s, "R %li %li\r\n", start, len);
	reply = lpc_write_c(fd, s, 2); /* echo and error code */
	if (!binary) printf("begin 644 LPC%i.data\n", dev->name);
	nline = 0;
	while (lpc_fd_gets(fd, s, 80) > 0) {
		/* trim the line */
		lpc_trim(s);
		/* if it is the checksum, ack and continue reading */
		if (sscanf(s, "%i%s", &i, t)==1) {
			V(".");
			lpc_write_c(fd, "OK\r\n", 1);
			continue;
		}
		/* uuencode data */
		if (binary) {
			i = lpc_uudecode(s, t);
			if (fwrite(t, 1, i, stdout) != i)
				exit(2); /* bah! */
		} else {
			printf("%s\n", s);
		}
		nline++;
	}
	V("\n");
	if (!binary) printf("`\nend\n");

	exit(nline ? 0 : 1);
}
