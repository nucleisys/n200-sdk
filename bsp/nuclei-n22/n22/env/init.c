//See LICENSE for license details.
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

#include "n22/drivers/riscv_encoding.h"
#include "n22/drivers/n22_func.h"


  // Need to include the soc_init function, so need to include the soc_func.h here
#include "soc/drivers/soc_func.h"


void _init()
{
  #ifndef NO_INIT
  soc_init();

  printf("Core freq at %d Hz\n", get_cpu_freq());


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
