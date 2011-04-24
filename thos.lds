OUTPUT_FORMAT("elf32-littlearm")
OUTPUT_ARCH(arm)
ENTRY(_thos_start)

SECTIONS
{
	. = 0x10000400;
	.text : {
		*(.boot)
		*(.text)
	}
	.rodata : { *(.rodata) }
	.data : { *(.data) }

	.bss : {
		. = ALIGN(16);
		__bss_start = .;
		*(.bss);
		. = ALIGN(16);
		__bss_end = .;
	}
}
