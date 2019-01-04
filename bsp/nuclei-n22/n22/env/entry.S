// See LICENSE for license details

#ifndef ENTRY_S
#define ENTRY_S

#include "n22/drivers/riscv_encoding.h"
#include "n22/drivers/riscv_bits.h"
#include "n22/drivers/n22_clic.h"
#include "n22/drivers/n22_tmr.h"

###############################################
###############################################
# Disable Interrupt
#
.macro DISABLE_MIE
  csrc CSR_MSTATUS, MSTATUS_MIE  
.endm


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
// exception entry point
//
  .section      .text.exception	
  .align 6// In CLIC mode, the trap entry must be 64bytes aligned
  .global exception_entry
.weak exception_entry
exception_entry:
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
// trap entry point
//
  .section      .text.trap	
  .equ FRAMESIZE, 256
  .align 6
  .global trap_entry
.weak trap_entry
trap_entry:
  addi sp, sp, -FRAMESIZE // Allocate space on stack. 
  sw a1, 11*REGBYTES(sp)       // Save a1.
  csrr a1, CSR_MCAUSE         // Get mcause of interrupted context.
  sw a0, 10*REGBYTES(sp)       // Save a0.
  csrr a0, CSR_MEPC           // Get mepc of interrupt context.
  bgez a1, exception_entry     // Handle synchronous exception. 
  sw a0, 32*REGBYTES(sp)       // Save mepc.
  sw a1, 33*REGBYTES(sp)       // Save mcause of interrupted context.
  sw a2, 12*REGBYTES(sp)    // Save other argument registers.
  sw a3, 13*REGBYTES(sp)
  sw a4, 14*REGBYTES(sp)
  sw a5, 15*REGBYTES(sp)
  sw a6, 16*REGBYTES(sp)
  sw a7, 17*REGBYTES(sp)
  sw t0, 5*REGBYTES(sp)    // Save temporaries.
  sw t1, 6*REGBYTES(sp)
  sw t2, 7*REGBYTES(sp)
  sw t3, 28*REGBYTES(sp)
  sw t4, 29*REGBYTES(sp)
  sw t5, 30*REGBYTES(sp)
  sw t6, 31*REGBYTES(sp)
  sw ra, 1*REGBYTES(sp)   // Save return address 
  csrrsi a0, CSR_MNXTI, MSTATUS_MIE   // Get highest current interrupt and enable interrupts.
                            // Will return original interrupt if no others appear. 
  //----Interrupts enabled ---------------------
  beqz a0, exit           // Check if original interrupt vanished. 

int_loop:             // 5 instructions in pending-interrupt service loop.
  lw a1, (a0)             // Indirect into handler vector table for function pointer. 
  csrrsi x0, CSR_MSTATUS, MSTATUS_MIE // Ensure interrupts enabled. 

  jalr a1                 // Call C ABI Routine, a0 has interrupt ID encoded. 
                          // Routine must clear down interrupt in CLIC.
  csrrsi a0, CSR_MNXTI, MSTATUS_MIE   // Claim any pending interrupt at level > mcause.pil 
  bnez a0, int_loop   // Loop to service any interrupt. 

//--- Restore ABI registers with interrupts enabled -
  lw ra, 1*REGBYTES(sp)    // Restore return address
  lw t0, 5*REGBYTES(sp)    // Restore temporaries.
  lw t1, 6*REGBYTES(sp)
  lw t2, 7*REGBYTES(sp)
  lw t3, 28*REGBYTES(sp)
  lw t4, 29*REGBYTES(sp)
  lw t5, 30*REGBYTES(sp)
  lw t6, 31*REGBYTES(sp)
  lw a2, 12*REGBYTES(sp)    // Restore other arguments.
  lw a3, 13*REGBYTES(sp)
  lw a4, 14*REGBYTES(sp)
  lw a5, 15*REGBYTES(sp)
  lw a6, 16*REGBYTES(sp)
  lw a7, 17*REGBYTES(sp)
  lw a1, 33*REGBYTES(sp)       // Get saved mcause,
exit:                     // Fast exit point.
  lw a0, 32*REGBYTES(sp)       // Get saved mepc.

  csrrci x0, CSR_MSTATUS, MSTATUS_MIE // Disable interrupts 
//---- Critical section with interrupts disabled -----------------------
  csrw CSR_MCAUSE, a1         // Restore previous context.

  lw a1, 11*REGBYTES(sp)       // Restore original a1 value.
  csrw mepc, a0           // Restore previous context.

  csrrci a0, CSR_MNXTI, MSTATUS_MIE   // Claim highest current interrupt. 
  bnez a0, int_loop   // Go around if new interrupt.

  lw a0, 10*REGBYTES(sp)       // Restore original a0 value.
  addi sp, sp, FRAMESIZE  // Reclaim stack space.
  mret                    // Return from interrupt.

###############################################
###############################################
// IRQ entry point
//
  .section      .text.irq	
  .align 2
  .global irq_entry
.weak irq_entry
irq_entry: // -------------> This label will be set to MTVT2 register
  // Allocate the stack space
  addi sp, sp, -18*REGBYTES

  SAVE_CONTEXT// Save 16 regs

  //------This special CSR read operation, which is actually use mcause as operand to directly store it to memory
  csrrwi  x0, CSR_PUSHCAUSE, 16
  //------This special CSR read operation, which is actually use mepc as operand to directly store it to memory
  csrrwi  x0, CSR_PUSHMEPC, 17
 
service_loop:
  //------This special CSR read/write operation, which is actually Claim the CLIC to find its pending highest
  // ID, if the ID is not 0, then automatically enable the mstatus.MIE, and jump to its vector-entry-label, and
  // update the link register 
  csrrw ra, CSR_JLMNXTI, ra 
  
  //TODO: this post checking can be removed
  csrr a0, CSR_MNXTI   # Check any pending interrupt at level > mcause.pil
  bnez a0, service_loop   # Loop to service any interrupt.
 
  RESTORE_CONTEXT

  #---- Critical section with interrupts disabled -----------------------
  DISABLE_MIE # Disable interrupts 

  LOAD x5,  17*REGBYTES(sp)
  csrw CSR_MEPC, x5  
  LOAD x5,  16*REGBYTES(sp)
  csrw CSR_MCAUSE, x5  


  // De-allocate the stack space
  addi sp, sp, 18*REGBYTES
  // Return to regular code
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
  // Allocate the stack space
  addi sp, sp, -18*REGBYTES

  // Save the caller saving registers (context)
  SAVE_CONTEXT
  // Save the MEPC/NStatus reg
  SAVE_MEPC_MSTATUS

     // Call the function
  call handle_nmi

  // Restore the MEPC/NStatus reg
  RESTORE_MEPC_MSTATUS
  // Restore the caller saving registers (context)
  RESTORE_CONTEXT

  // De-allocate the stack space
  addi sp, sp, 18*REGBYTES
  // Return to regular code
  mret



	
#endif
