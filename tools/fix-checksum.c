#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>

int main(int argc, char **argv)
{
    int i, fd;
    void *mapaddr;
    uint32_t sum=0, *data;

    if (argc != 2) {
	fprintf(stderr, "%s: pass a binary filename please\n", argv[0]);
	exit(1);
    }
    fd = open(argv[1], O_RDWR);
    if (fd < 0) {
	fprintf(stderr, "%s: %s: %s\n", argv[0], argv[1], strerror(errno));
	exit(1);
    }
    mapaddr = mmap(0, 0x20, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (mapaddr == (typeof(mapaddr))-1) {
	fprintf(stderr, "%s: %s: %s\n", argv[0], argv[1], strerror(errno));
	exit(1);
    }
    data = mapaddr;

    for (i = 0; i < 7; i++)
	sum += data[i];
    data[i] = -sum;
    exit(0);
}
