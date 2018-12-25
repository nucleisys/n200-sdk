// See LICENSE for license details.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "n200/drivers/riscv_encoding.h"
#include "n200/drivers/n200_func.h"

    // Configure PMP to make all the address space accesable and executable
void pmp_open_all_space(){
    // Config entry0 addr to all 1s to make the range cover all space
    asm volatile ("li x6, 0xffffffff":::"x6");
    asm volatile ("csrw pmpaddr0, x6":::);
    // Config entry0 cfg to make it NAPOT address mode, and R/W/X okay
    asm volatile ("li x6, 0x7f":::"x6");
    asm volatile ("csrw pmpcfg0, x6":::);
}

void switch_m2u_mode(){
    clear_csr (mstatus,MSTATUS_MPP);
    //printf("\nIn the m2u function, the mstatus is 0x%x\n", read_csr(mstatus));
    //printf("\nIn the m2u function, the mepc is 0x%x\n", read_csr(mepc));
    asm volatile ("la x6, 1f    ":::"x6");
    asm volatile ("csrw mepc, x6":::);
    asm volatile ("mret":::);
    asm volatile ("1:":::);
} 

uint32_t mtime_lo(void)
{
  return *(volatile uint32_t *)(TMR_CTRL_ADDR + TMR_MTIME);
}


uint32_t mtime_hi(void)
{
  return *(volatile uint32_t *)(TMR_CTRL_ADDR + TMR_MTIME + 4);
}

uint64_t get_timer_value()
{
  while (1) {
    uint32_t hi = mtime_hi();
    uint32_t lo = mtime_lo();
    if (hi == mtime_hi())
      return ((uint64_t)hi << 32) | lo;
  }
}

uint32_t get_timer_freq()
{
  return TMR_FREQ;
}

uint64_t get_instret_value()
{
  while (1) {
    uint32_t hi = read_csr(minstreth);
    uint32_t lo = read_csr(minstret);
    if (hi == read_csr(minstreth))
      return ((uint64_t)hi << 32) | lo;
  }
}

uint64_t get_cycle_value()
{
  while (1) {
    uint32_t hi = read_csr(mcycleh);
    uint32_t lo = read_csr(mcycle);
    if (hi == read_csr(mcycleh))
      return ((uint64_t)hi << 32) | lo;
  }
}

uint32_t __attribute__((noinline)) measure_cpu_freq(size_t n)
{
  uint32_t start_mtime, delta_mtime;
  uint32_t mtime_freq = get_timer_freq();

  // Don't start measuruing until we see an mtime tick
  uint32_t tmp = mtime_lo();
  do {
    start_mtime = mtime_lo();
  } while (start_mtime == tmp);

  uint32_t start_mcycle = read_csr(mcycle);

  do {
    delta_mtime = mtime_lo() - start_mtime;
  } while (delta_mtime < n);

  uint32_t delta_mcycle = read_csr(mcycle) - start_mcycle;

  return (delta_mcycle / delta_mtime) * mtime_freq
         + ((delta_mcycle % delta_mtime) * mtime_freq) / delta_mtime;
}

uint32_t get_cpu_freq()
{
  uint32_t cpu_freq;

  // warm up
  measure_cpu_freq(1);
  // measure for real
  cpu_freq = measure_cpu_freq(100);

  return cpu_freq;
}



// Note that there are no assertions or bounds checking on these
// parameter values.

void pic_set_threshold (
			 uint32_t threshold){

  volatile uint32_t* threshold_ptr = (uint32_t*) (PIC_CTRL_ADDR +
                                                              PIC_THRESHOLD_OFFSET
                                                              );

  *threshold_ptr = threshold;

}
  

void pic_enable_interrupt (uint32_t source){

  volatile uint32_t * current_ptr = (volatile uint32_t *)(PIC_CTRL_ADDR +
                                                        PIC_ENABLE_OFFSET +
                                                        ((source >> 3) & (~0x3))//Source number divide 32 and then multip 4 (bytes)
                                                        );
  uint32_t current = *current_ptr;
  current = current | ( 1 << (source & 0x1f));// Only check the least 5 bits
  *current_ptr = current;

}

void pic_disable_interrupt (uint32_t source){
  
  volatile uint32_t * current_ptr = (volatile uint32_t *) (PIC_CTRL_ADDR +
                                                         PIC_ENABLE_OFFSET +
                                                         ((source >> 3) & (~0x3))//Source number divide 32 and then multip 4 (bytes)
                                                          );
  uint32_t current = *current_ptr;
  current = current & ~(( 1 << (source & 0x1f)));// Only check the least 5 bits
  *current_ptr = current;
  
}

void pic_set_priority (uint32_t source, uint32_t priority){

  if (PIC_NUM_PRIORITIES > 0) {
    volatile uint32_t * priority_ptr = (volatile uint32_t *)
      (PIC_CTRL_ADDR +
       PIC_PRIORITY_OFFSET +
       (source << PIC_PRIORITY_SHIFT_PER_SOURCE));// Each priority reg occupy a word, so multiple 2
    *priority_ptr = priority;
  }
}

uint32_t pic_claim_interrupt(){
  

  volatile uint32_t * claim_addr = (volatile uint32_t * )
    (PIC_CTRL_ADDR +
     PIC_CLAIM_OFFSET 
     );

  return  *claim_addr;
  
}

uint32_t pic_check_eip(){
  

  volatile uint32_t * eip_addr = (volatile uint32_t * )
    (PIC_CTRL_ADDR +
     PIC_EIP_OFFSET 
     );

  return  *eip_addr;
  
}

void pic_complete_interrupt(uint32_t source){
  
  volatile uint32_t * claim_addr = (volatile uint32_t *) (PIC_CTRL_ADDR +
                                                                PIC_CLAIM_OFFSET  
                                                                );
  *claim_addr = source;
  
}



