.section .boot, "ax"

.global _thos_start
.extern __bss_start
.extern __bss_end
.extern thos_setup
.extern thos_main

_thos_start:
/* set the stack pointer */
	mov r0, pc
	sub r0, #12
	mov sp, r0

/* clear BSS */
	mov r0, #0
	ldr r1, =__bss_start
	ldr r2, =__bss_end
0:	
	cmp r1, r2
	bge 1f
	str r0, [r1]
	add r1, #4
	b 0b

1:	bl thos_setup
	bl thos_main
	b 1b
