OUTPUT_FORMAT("elf32-littlearm")
OUTPUT_ARCH(arm)
ENTRY(_thos_start)

regs = 0;
jiffies = 0x40018008;

SECTIONS
{
	. = 0x10000400;
	.text : {
		*(.boot)
		*(.text)
	}
	.rodata : { *(.rodata) }
	.data : { *(.data) }

	__task_begin = .;
	.task : {*(.task) }
	__task_end = .;

	.bss : {
		. = ALIGN(16);
		__bss_start = .;
		*(.bss);
		. = ALIGN(16);
		__bss_end = .;
	}
}
