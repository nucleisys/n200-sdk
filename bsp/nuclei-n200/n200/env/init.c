//See LICENSE for license details.
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

#include "n200/drivers/riscv_encoding.h"
#include "n200/drivers/n200_func.h"


  // Need to include the soc_init function, so need to include the soc_func.h here
#include "soc/drivers/soc_func.h"

extern void trap_entry();
extern void nmi_entry();
extern void irq_entry();




void _init()
{
  #ifndef NO_INIT
  soc_init();

  
  write_csr(mtvec, &trap_entry);
     // The N200 self-defined CSR (not standard RISC-V CSR) must use this function style
  write_csr_mivec(&irq_entry);
  write_csr_mnvec(&nmi_entry);


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
