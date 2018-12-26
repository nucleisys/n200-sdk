// See LICENSE for license details.

// See LICENSE for license details.

#include "n22/drivers/riscv_encoding.h"

	.section .init
	.globl _start
	.type _start,@function

_start:
	csrc mstatus, MSTATUS_MIE  //Disable the interrupt globally
.option push
.option norelax
	la gp, __global_pointer$
.option pop
	la sp, _sp

//TODO    // Check mcause, If it is not 0, then it is NMI, jump to NMI handler
//TODO    csrr t0, mcause
//TODO    beq t0, zero, 1f // If it is 0, then from reset, just bypass the NMI jumping
//TODO    la t0, nmi_entry
//TODO    jr t0
//TODO1:


	/* Bob: Load code section from flash to ILM */
	la a0, _ilm_lma
	la a1, _ilm
    beq a0, a1, 2f  /*If the ILM phy-address same as the logic-address, then quit*/
	la a2, _eilm
	bgeu a1, a2, 2f
1:
	lw t0, (a0)
	sw t0, (a1)
	addi a0, a0, 4
	addi a1, a1, 4
	bltu a1, a2, 1b
2:

	/* Load data section */
	la a0, _data_lma
	la a1, _data
	la a2, _edata
	bgeu a1, a2, 2f
1:
	lw t0, (a0)
	sw t0, (a1)
	addi a0, a0, 4
	addi a1, a1, 4
	bltu a1, a2, 1b
2:

	/* Clear bss section */
	la a0, __bss_start
	la a1, _end
	bgeu a0, a1, 2f
1:
	sw zero, (a0)
	addi a0, a0, 4
	bltu a0, a1, 1b
2:

	/* Intial the mtvec/mtvt/mtvt2 TODO */

	/* Call global constructors */
	la a0, __libc_fini_array
	call atexit
	call __libc_init_array

#ifdef __riscv_flen
	/* Enable FPU */
	li t0, MSTATUS_FS
	csrs mstatus, t0 
	csrw fcsr, x0   
#endif

	/* argc = argv = 0 */
	li a0, 0
	li a1, 0
	call main
	tail exit

1:
	j 1b
