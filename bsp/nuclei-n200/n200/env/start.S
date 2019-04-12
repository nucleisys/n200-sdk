// See LICENSE for license details.

// See LICENSE for license details.

#include "n200/drivers/riscv_encoding.h"

	.section .init


    .weak  eclic_msip_handler 

    .weak  eclic_mtip_handler 

    .weak  eclic_meip_handler 

    .weak  eclic_irq19_handler

    .weak  eclic_irq20_handler
    .weak  eclic_irq21_handler
    .weak  eclic_irq22_handler
    .weak  eclic_irq23_handler

    .weak  eclic_irq24_handler
    .weak  eclic_irq25_handler
    .weak  eclic_irq26_handler
    .weak  eclic_irq27_handler

    .weak  eclic_irq28_handler
    .weak  eclic_irq29_handler
    .weak  eclic_irq30_handler
    .weak  eclic_irq31_handler

    .weak  eclic_irq32_handler
    .weak  eclic_irq33_handler
    .weak  eclic_irq34_handler
    .weak  eclic_irq35_handler

    .weak  eclic_irq36_handler
    .weak  eclic_irq37_handler
    .weak  eclic_irq38_handler
    .weak  eclic_irq39_handler

    .weak  eclic_irq40_handler
    .weak  eclic_irq41_handler
    .weak  eclic_irq42_handler
    .weak  eclic_irq43_handler

    .weak  eclic_irq44_handler
    .weak  eclic_irq45_handler
    .weak  eclic_irq46_handler
    .weak  eclic_irq47_handler

    .weak  eclic_irq48_handler
    .weak  eclic_irq49_handler
    .weak  eclic_irq50_handler

vector_base:
    j _start 
    .align 2
    .word  0 
    .word  0 
    .word  eclic_msip_handler 

    .word  0 
    .word  0 
    .word  0 
    .word  eclic_mtip_handler 

    .word  0 
    .word  0 
    .word  0 
    .word  0 

    .word  0
    .word  0
    .word  0
    .word  0

    .word  0
    .word  0
    .word  0
    .word  eclic_irq19_handler

    .word  eclic_irq20_handler
    .word  eclic_irq21_handler
    .word  eclic_irq22_handler
    .word  eclic_irq23_handler

    .word  eclic_irq24_handler
    .word  eclic_irq25_handler
    .word  eclic_irq26_handler
    .word  eclic_irq27_handler

    .word  eclic_irq28_handler
    .word  eclic_irq29_handler
    .word  eclic_irq30_handler
    .word  eclic_irq31_handler

    .word  eclic_irq32_handler
    .word  eclic_irq33_handler
    .word  eclic_irq34_handler
    .word  eclic_irq35_handler

    .word  eclic_irq36_handler
    .word  eclic_irq37_handler
    .word  eclic_irq38_handler
    .word  eclic_irq39_handler

    .word  eclic_irq40_handler
    .word  eclic_irq41_handler
    .word  eclic_irq42_handler
    .word  eclic_irq43_handler

    .word  eclic_irq44_handler
    .word  eclic_irq45_handler
    .word  eclic_irq46_handler
    .word  eclic_irq47_handler

    .word  eclic_irq48_handler
    .word  eclic_irq49_handler
    .word  eclic_irq50_handler

	.globl _start
	.type _start,@function
_start:

    /* Set the the NMI base to share with mtvec by setting CSR_MMISC_CTL */
    li t0, (0x1 << 9);
    csrs CSR_MMISC_CTL, t0

	/* Intial the mtvt*/
    la t0, vector_base
    csrw CSR_MTVT, t0

	/* Intial the mtvt2 and enable it*/
    la t0, irq_entry
    csrw CSR_MTVT2, t0
    csrs CSR_MTVT2, 0x1

    /* Intial the CSR MTVEC for the Trap ane NMI base addr*/
    la t0, trap_entry
    csrw CSR_MTVEC, t0


#ifdef __riscv_flen
	/* Enable FPU */
	li t0, MSTATUS_FS
	csrs mstatus, t0 
	csrw fcsr, x0   
#endif

    .option push
    .option norelax
	la gp, __global_pointer$
    .option pop
	la sp, _sp

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

	/* Call global constructors */
	la a0, __libc_fini_array
	call atexit
	call __libc_init_array


	/* argc = argv = 0 */
	li a0, 0
	li a1, 0
	call main
	tail exit

1:
	j 1b

	.global disable_mcycle_minstret
disable_mcycle_minstret:
    csrsi CSR_MCOUNTINHIBIT, 0x5
	ret

	.global enable_mcycle_minstret
enable_mcycle_minstret:
    csrci CSR_MCOUNTINHIBIT, 0x5
	ret
