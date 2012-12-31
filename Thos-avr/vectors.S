/* We need to provide interrupt vectors here */
.extern _thos_start
.extern do_jiffies_irq
.section .vectors, "ax"

	rjmp _thos_start
	
	/* We use one interrupt only, to count jiffies: always jump there */
	.rep 25
	nop
	.endr

	rjmp do_jiffies_irq
