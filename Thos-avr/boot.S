.section .boot, "ax"

.global _thos_start
.extern __data_load_start
.extern __data_load_end
.extern __data_start
.extern __bss_start
.extern __bss_end
_thos_start:
	/* r1  must always be 0 by convention, and clear irq in status  */
	eor r1, r1
	out 0x3f, r1
	/* set SP at address 0x3d, 0x3e -- assume 1k RAM + 0x60 for regs */
	ldi r28, lo8(1023 + 0x60)
	ldi r29, hi8(1023 + 0x60)
	out 0x3d, r28
	out 0x3e, r29

	/* Then, we must copy data from program memory to RAM */
_copy_data:
	/* Z (r30, 31) loads from program memory */
	ldi r30, lo8(__data_load_start)
	ldi r31, hi8(__data_load_start)
	/* "cpc" only compares with a register, so save high part here */
	ldi r16, hi8(__data_load_end)
	/* X (r26, r27) stores in RAM */
	ldi r26, lo8(__data_start)
	ldi r27, hi8(__data_start)

	/* __data_load_end (and start) is provided by linker script */
0:	cpi r30, lo8(__data_load_end)
	cpc r31, r16
	brge 1f
	lpm r2, Z+
	st X+, r2
	rjmp 0b

1:	/* Now, clear bss, use X again, and r16 for high part of end */
	ldi r26, lo8(__bss_start)
	ldi r27, hi8(__bss_start)
	ldi r16, hi8(__bss_end)
0:	cpi r26, lo8(__bss_end)
	cpc r27, r16
	brge 1f
	st X+, r1
	rjmp 0b
	
1:	/* Finally, run real code */
	rcall thos_setup
	sei
	rcall thos_main
	rjmp _thos_start
