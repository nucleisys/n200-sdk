// See LICENSE for license details

#ifndef ENTRY_S
#define ENTRY_S

#include "n22/drivers/riscv_encoding.h"
#include "n22/drivers/riscv_bits.h"
#include "n22/drivers/n22_clic.h"
#include "n22/drivers/n22_tmr.h"



###############################################
###############################################
#Save caller registers
.macro SAVE_CONTEXT

  STORE x1,  0*REGBYTES(sp)
  STORE x5,  1*REGBYTES(sp)
  STORE x6,  2*REGBYTES(sp)
  STORE x7,  3*REGBYTES(sp)
  STORE x10, 4*REGBYTES(sp)
  STORE x11, 5*REGBYTES(sp)
  STORE x12, 6*REGBYTES(sp)
  STORE x13, 7*REGBYTES(sp)
  STORE x14, 8*REGBYTES(sp)
  STORE x15, 9*REGBYTES(sp)
  STORE x16, 10*REGBYTES(sp)
  STORE x17, 11*REGBYTES(sp)
  STORE x28, 12*REGBYTES(sp)
  STORE x29, 13*REGBYTES(sp)
  STORE x30, 14*REGBYTES(sp)
  STORE x31, 15*REGBYTES(sp)
.endm


###############################################
###############################################
#restore caller registers
.macro RESTORE_CONTEXT
  LOAD x1,  0*REGBYTES(sp)
  LOAD x5,  1*REGBYTES(sp)
  LOAD x6,  2*REGBYTES(sp)
  LOAD x7,  3*REGBYTES(sp)
  LOAD x10, 4*REGBYTES(sp)
  LOAD x11, 5*REGBYTES(sp)
  LOAD x12, 6*REGBYTES(sp)
  LOAD x13, 7*REGBYTES(sp)
  LOAD x14, 8*REGBYTES(sp)
  LOAD x15, 9*REGBYTES(sp)
  LOAD x16, 10*REGBYTES(sp)
  LOAD x17, 11*REGBYTES(sp)
  LOAD x28, 12*REGBYTES(sp)
  LOAD x29, 13*REGBYTES(sp)
  LOAD x30, 14*REGBYTES(sp)
  LOAD x31, 15*REGBYTES(sp)

.endm

###############################################
###############################################
# Save the mepc and mstatus
#
.macro SAVE_MEPC_MSTATUS
  csrr x5, CSR_MEPC  
  STORE x5,  16*REGBYTES(sp)
  csrr x5, CSR_MSTATUS  
  STORE x5,  17*REGBYTES(sp)
.endm

###############################################
###############################################
# Restore the mepc and mstatus
#
.macro RESTORE_MEPC_MSTATUS
  LOAD x5,  16*REGBYTES(sp)
  csrw CSR_MEPC, x5  
  LOAD x5,  17*REGBYTES(sp)
  csrw CSR_MSTATUS, x5  
.endm


###############################################
###############################################
# Disable Interrupt
#
.macro DISABLE_MIE
  csrc CSR_MSTATUS, MSTATUS_MIE  
.endm

###############################################
###############################################
# Enable Interrupt
#
.macro ENABLE_MIE
  csrs CSR_MSTATUS, MSTATUS_MIE  
.endm

###############################################
###############################################
// Trap entry point
//
  .section      .text.trap	
  .align 2
  .global trap_entry
.weak trap_entry
trap_entry:
  // Allocate the stack space
  addi sp, sp, -18*REGBYTES

  // Save the caller saving registers (context)
  SAVE_CONTEXT
  // Save the MEPC/MStatus reg
  SAVE_MEPC_MSTATUS

     // Set the function argument
  csrr a0, mcause
  mv a1, sp
     // Call the function
  call handle_trap

  // Restore the MEPC/MStatus reg
  RESTORE_MEPC_MSTATUS
  // Restore the caller saving registers (context)
  RESTORE_CONTEXT

  // De-allocate the stack space
  addi sp, sp, 18*REGBYTES
  // Return to regular code
  mret

###############################################
###############################################
// IRQ entry point
//
  .section      .text.irq	
  .align 2
  .global irq_entry
.weak irq_entry
irq_entry:
//TODO  // Allocate the stack space
//TODO  addi sp, sp, -18*REGBYTES
//TODO
//TODO  // Save the caller saving registers (context)
//TODO  SAVE_CONTEXT
//TODO  // Save the MIPC/IStatus reg
//TODO  SAVE_MIPC_ISTATUS
//TODO
//TODOcall_irq:
//TODO  //Claim the IRQ
//TODO  li t0, (PIC_CTRL_ADDR + PIC_CLAIM_OFFSET)
//TODO  lw a0, 0(t0)
//TODO
//TODO  // Call the handler function
//TODO  // a0 as the function argument
//TODO  call handle_irq
//TODO
//TODO  //Complete the IRQ
//TODO  // a0 as the return value
//TODO  li t0, (PIC_CTRL_ADDR + PIC_CLAIM_OFFSET)
//TODO  sw a0, 0(t0)
//TODO
//TODO
//TODO  // Check if there is pending irq to support interrupt tailing
//TODO  li x5, (PIC_CTRL_ADDR + PIC_EIP_OFFSET)// Use t0(x5) as the address register
//TODO  lw x5, 0(x5)// Read the value from PIC EIP reg
//TODO  bnez x5, call_irq // If the EIP reg is not zero, means there is a pending register
//TODO
//TODO  // Restore the MIPC/IStatus reg
//TODO  RESTORE_MIPC_ISTATUS
//TODO  // Restore the caller saving registers (context)
//TODO  RESTORE_CONTEXT
//TODO
//TODO  // De-allocate the stack space
//TODO  addi sp, sp, 18*REGBYTES
//TODO  // Return to regular code
  mret

###############################################
###############################################
// NMI entry point
//
  .section      .text.nmi	
  .align 2
  .global nmi_entry
.weak nmi_entry
nmi_entry:
  //TODO // Allocate the stack space
  //TODO addi sp, sp, -18*REGBYTES

  //TODO // Save the caller saving registers (context)
  //TODO SAVE_CONTEXT
  //TODO // Save the MNPC/NStatus reg
  //TODO SAVE_MNPC_NSTATUS

  //TODO    // Call the function
  //TODO call handle_nmi

  //TODO // Restore the MNPC/NStatus reg
  //TODO RESTORE_MNPC_NSTATUS
  //TODO // Restore the caller saving registers (context)
  //TODO RESTORE_CONTEXT

  //TODO // De-allocate the stack space
  //TODO addi sp, sp, 18*REGBYTES
  //TODO // Return to regular code
  mret



	
#endif
