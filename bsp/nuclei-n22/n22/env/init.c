//See LICENSE for license details.
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

#include "n22/drivers/riscv_encoding.h"
#include "n22/drivers/n22_func.h"
#include "n22/drivers/clic_driver.h"


  // Need to include the soc_init function, so need to include the soc_func.h here
#include "soc/drivers/soc_func.h"

interrupt_function_ptr_t localISR[CLIC_NUM_INTERRUPTS] __attribute__((aligned(512)));

void _init()
{
  #ifndef NO_INIT
  soc_init();// We must put this first before use any printf later

  printf("**************************************\n");
  printf("**************************************\n");
  printf("*                                    *\n");
  printf("Core freq at %d Hz\n", get_cpu_freq());
  printf("*                                    *\n");
  printf("**************************************\n");
  printf("**************************************\n");

  //ECLIC init 
  eclic_init(CLIC_NUM_INTERRUPTS);
  eclic_mode_enable();

  printf("\n\n\nAfter ECLIC mode enabled, the mtvec value is %x \n\n\n", read_csr(mtvec));




  // // It must be NOTED: 
  //  //    * In the RISC-V arch, if user mode and PMP supported, then by default if PMP is not configured 
  //  //      with valid entries, then user mode cannot access any memory, and cannot execute any instructions.
  //  //    * So if switch to user-mode and still want to continue, then you must configure PMP first
  //pmp_open_all_space();
  //switch_m2u_mode();

  #endif
}

void _fini()
{
}
