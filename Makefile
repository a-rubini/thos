# Cross compiling:
AS              = $(CROSS_COMPILE)as
LD              = $(CROSS_COMPILE)ld
CC              = $(CROSS_COMPILE)gcc
CPP             = $(CC) -E
AR              = $(CROSS_COMPILE)ar
NM              = $(CROSS_COMPILE)nm
STRIP           = $(CROSS_COMPILE)strip
OBJCOPY         = $(CROSS_COMPILE)objcopy
OBJDUMP         = $(CROSS_COMPILE)objdump

# Using thumb for version 7 ARM core:
CFLAGS  = -march=armv7-m -mthumb -g -Wall -ffreestanding -O2
ASFLAGS = -march=armv7-m -mthumb -g -Wall

# Use our own linker script
LDFLAGS = -T thos.lds

# Task source files and objects
TSRC = $(wildcard task-*.c)
TOBJ = $(TSRC:.c=.o)

# Our target
thos.bin: thos
	$(OBJCOPY) -O binary $^ $@

thos: boot.o io.o main.o $(TOBJ)
	$(LD) $(LDFLAGS) $^ -o $@

clean:
	rm -f thos.bin thos *.o *~