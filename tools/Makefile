CFLAGS = -ggdb -Wall -O2
LDFLAGS = -L. -llpclib

HOSTCC ?= gcc

%.o: %.c
	$(HOSTCC) $(CFLAGS) -c $*.c -o $*.o

%: %.c
	$(HOSTCC) $(CFLAGS) $*.c $(LDFLAGS) -o $@

LIB = liblpclib.a
LOBJ = lpclib.o

PROG = dump program progrom fix-checksum

all: $(PROG)

$(PROG): $(LIB)

$(LIB): $(LOBJ)
	ar rc $@ $<

clean:
	rm -f $(LIB) $(PROG) *.o *~