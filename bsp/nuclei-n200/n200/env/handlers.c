//See LICENSE for license details.
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
#include "n200/drivers/riscv_encoding.h"
#include "n200/drivers/n200_func.h"

__attribute__((weak)) uintptr_t handle_trap(uintptr_t mcause, uintptr_t sp)
{
  write(1, "trap\n", 5);
  uint32_t mstatus_mps_bits = ((read_csr(mstatus) & MSTATUS_MPS) >> MSTATUS_MPS_LSB);
  printf("In trap handler, the msubmode is 0x%x\n", read_csr_msubmode);
  printf("In trap handler, the mstatus.MPS is 0x%x\n", mstatus_mps_bits);
  printf("In trap handler, the mcause is %d\n", mcause);
  printf("In trap handler, the mepc is 0x%x\n", read_csr(mepc));
  printf("In trap handler, the mtval is 0x%x\n", read_csr(mbadaddr));
  if(mstatus_mps_bits == 0x1) {
      printf("The exception is happened from previous Exception mode, hence is Double-Exception-fault!\n");
  } else if (mstatus_mps_bits == 0x2){
      printf("The exception is happened from previous NMI mode!\n");
  } else if (mstatus_mps_bits == 0x3){
      printf("The exception is happened from previous IRQ mode!\n");
  } 
  _exit(mcause);
  return 0;
}


/*Entry Point for PIC Interrupt Handler*/
__attribute__((weak)) uint32_t handle_irq(uint32_t int_num){
    // Enable interrupts to allow interrupt preempt based on priority
    set_csr(mstatus, MSTATUS_MIE);
  pic_interrupt_handlers[int_num]();
    // Disable interrupts 
    clear_csr(mstatus, MSTATUS_MIE);
  return int_num;
}


__attribute__((weak)) uintptr_t handle_nmi()
{
  write(1, "nmi\n", 5);
  _exit(1);
  return 0;
}


