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

# We must define what is the target MCU. gcc and avrdudue use different names
MCU ?= atmega8
PMCU ?= m8
CFLAGS  = -mmcu=$(MCU) -g -Wall -ffreestanding -Os
ASFLAGS = -mmcu=$(MCU) -g -Wall

# Use our own linker script
LDFLAGS = -T thos.lds

# Task source files and objects
TSRC = $(wildcard task-*.c)
TOBJ = $(TSRC:.c=.o)

# Our targets
all: thos.bin thos.hex

thos.bin: thos
	$(OBJCOPY) -O binary $^ $@

thos.hex: thos
	$(OBJCOPY) -O ihex $^ $@

thos: vectors.o boot.o jiffies.o io.o main.o $(TOBJ)
	$(LD) $(LDFLAGS) $^ -o $@

TTY ?= /dev/ttyUSB0
load:
	avrdude -p $(PMCU) -c stk500v2 -P $(TTY) -e -U flash:w:thos.hex

clean:
	rm -f thos.bin thos.hex thos *.o *~