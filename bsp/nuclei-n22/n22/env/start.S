// See LICENSE for license details.

// See LICENSE for license details.

#include "n22/drivers/riscv_encoding.h"

	.section .init


    .weak  clic_ssip_handler 
    .weak  clic_msip_handler 

    .weak  clic_utip_handler 
    .weak  clic_stip_handler 
    .weak  clic_mtip_handler 

    .weak  clic_ueip_handler 
    .weak  clic_seip_handler 
    .weak  clic_meip_handler 

    .weak  clic_imecci_handler
    .weak  clic_bwei_handler
    .weak  clic_pmovi_handler
    .weak  clic_irq19_handler

    .weak  clic_irq20_handler
    .weak  clic_irq21_handler
    .weak  clic_irq22_handler
    .weak  clic_irq23_handler

    .weak  clic_irq24_handler
    .weak  clic_irq25_handler
    .weak  clic_irq26_handler
    .weak  clic_irq27_handler

    .weak  clic_irq28_handler
    .weak  clic_irq29_handler
    .weak  clic_irq30_handler
    .weak  clic_irq31_handler

    .weak  clic_irq32_handler
    .weak  clic_irq33_handler
    .weak  clic_irq34_handler
    .weak  clic_irq35_handler

    .weak  clic_irq36_handler
    .weak  clic_irq37_handler
    .weak  clic_irq38_handler
    .weak  clic_irq39_handler

    .weak  clic_irq40_handler
    .weak  clic_irq41_handler
    .weak  clic_irq42_handler
    .weak  clic_irq43_handler

    .weak  clic_irq44_handler
    .weak  clic_irq45_handler
    .weak  clic_irq46_handler
    .weak  clic_irq47_handler

    .weak  clic_irq48_handler
    .weak  clic_irq49_handler
    .weak  clic_irq50_handler
    .weak  clic_irq51_handler

    .weak  clic_irq52_handler
    .weak  clic_irq53_handler
    .weak  clic_irq54_handler
    .weak  clic_irq55_handler

    .weak  clic_irq56_handler
    .weak  clic_irq57_handler
    .weak  clic_irq58_handler
    .weak  clic_irq59_handler

    .weak  clic_irq50_handler
    .weak  clic_irq61_handler
    .weak  clic_irq62_handler
    .weak  clic_irq63_handler

    .weak  clic_irq64_handler
    .weak  clic_irq65_handler
    .weak  clic_irq66_handler
    .weak  clic_irq67_handler

    .weak  clic_irq68_handler
    .weak  clic_irq69_handler
    .weak  clic_irq70_handler

vector_base:
    j _start 
    .align 2
    .word  clic_ssip_handler 
    .word  0 
    .word  clic_msip_handler 

    .word  clic_utip_handler 
    .word  clic_stip_handler 
    .word  0 
    .word  clic_mtip_handler 

    .word  clic_ueip_handler 
    .word  clic_seip_handler 
    .word  0 
    .word  clic_meip_handler 

    .word  0
    .word  0
    .word  0
    .word  0

    .word  clic_imecci_handler
    .word  clic_bwei_handler
    .word  clic_pmovi_handler
    .word  clic_irq19_handler

    .word  clic_irq20_handler
    .word  clic_irq21_handler
    .word  clic_irq22_handler
    .word  clic_irq23_handler

    .word  clic_irq24_handler
    .word  clic_irq25_handler
    .word  clic_irq26_handler
    .word  clic_irq27_handler

    .word  clic_irq28_handler
    .word  clic_irq29_handler
    .word  clic_irq30_handler
    .word  clic_irq31_handler

    .word  clic_irq32_handler
    .word  clic_irq33_handler
    .word  clic_irq34_handler
    .word  clic_irq35_handler

    .word  clic_irq36_handler
    .word  clic_irq37_handler
    .word  clic_irq38_handler
    .word  clic_irq39_handler

    .word  clic_irq40_handler
    .word  clic_irq41_handler
    .word  clic_irq42_handler
    .word  clic_irq43_handler

    .word  clic_irq44_handler
    .word  clic_irq45_handler
    .word  clic_irq46_handler
    .word  clic_irq47_handler

    .word  clic_irq48_handler
    .word  clic_irq49_handler
    .word  clic_irq50_handler
    .word  clic_irq51_handler

    .word  clic_irq52_handler
    .word  clic_irq53_handler
    .word  clic_irq54_handler
    .word  clic_irq55_handler

    .word  clic_irq56_handler
    .word  clic_irq57_handler
    .word  clic_irq58_handler
    .word  clic_irq59_handler

    .word  clic_irq50_handler
    .word  clic_irq61_handler
    .word  clic_irq62_handler
    .word  clic_irq63_handler

    .word  clic_irq64_handler
    .word  clic_irq65_handler
    .word  clic_irq66_handler
    .word  clic_irq67_handler

    .word  clic_irq68_handler
    .word  clic_irq69_handler
    .word  clic_irq70_handler

	.globl _start
	.type _start,@function
_start:
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

	/* Set the the NMI cause to 0x1001 by setting CSR_MMISC_CTL */
    li t0, 0x200;
    csrs CSR_MMISC_CTL, t0

	/* Intial the mtvt*/
    la t0, vector_base
    csrw CSR_MTVT, t0

	/* Intial the mtvt2 and enable it*/
    la t0, irq_entry
    csrw CSR_MIRQ_ENTRY, t0
    csrs CSR_MIRQ_ENTRY, 0x1

	/* Intial the mtvec*/
    la t0, trap_entry
    csrw CSR_MTVEC, t0

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
