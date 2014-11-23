/*Page Fault Handling ISR */

		.text
error:  .long 0
		.globl page_fault_isr, error

page_fault_isr:
		popl error
		pushfl
		cli
		pushal
		call page_fault
		popal
		popfl
		iret
