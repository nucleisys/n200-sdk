// See LICENSE for license details.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "n22/drivers/riscv_encoding.h"
#include "n22/drivers/n22_func.h"

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



  
void eclic_init ( uint32_t num_irq )
{

  typedef volatile uint32_t vuint32_t;

  //clear cfg register 
  *(volatile uint8_t*)(CLIC_ADDR_BASE+CLIC_CFG_OFFSET)=0;

  //clear minthresh register 
  *(volatile uint8_t*)(CLIC_ADDR_BASE+CLIC_MTH_OFFSET)=0;

  //clear all IP/IE/ATTR/CTRL bits for all interrupt sources
  vuint32_t * ptr;

  vuint32_t * base = (vuint32_t*)(CLIC_ADDR_BASE + CLIC_INT_IP_OFFSET);
  vuint32_t * upper = (vuint32_t*)(base + num_irq*4);

  for (ptr = base; ptr < upper; ptr=ptr+4){
    *ptr = 0;
  }
}



void eclic_enable_interrupt (uint32_t source) {
    *(volatile uint8_t*)(CLIC_ADDR_BASE+CLIC_INT_IE_OFFSET+source*4) = 1;
}

void eclic_disable_interrupt (uint32_t source){
    *(volatile uint8_t*)(CLIC_ADDR_BASE+CLIC_INT_IE_OFFSET+source*4) = 0;
}

void eclic_set_pending(uint32_t source){
    *(volatile uint8_t*)(CLIC_ADDR_BASE+CLIC_INT_IP_OFFSET+source*4) = 1;
}

void eclic_clear_pending(uint32_t source){
    *(volatile uint8_t*)(CLIC_ADDR_BASE+CLIC_INT_IP_OFFSET+source*4) = 0;
}

void eclic_set_intctrl (uint32_t source, uint8_t intctrl){
  *(volatile uint8_t*)(CLIC_ADDR_BASE+CLIC_INT_CTRL_OFFSET+source*4) = intctrl;
}

uint8_t eclic_get_intctrl  (uint32_t source){
  return *(volatile uint8_t*)(CLIC_ADDR_BASE+CLIC_INT_CTRL_OFFSET+source*4);
}

void eclic_set_intattr (uint32_t source, uint8_t intattr){
  *(volatile uint8_t*)(CLIC_ADDR_BASE+CLIC_INT_ATTR_OFFSET+source*4) = intattr;
}

uint8_t eclic_get_intattr  (uint32_t source){
  return *(volatile uint8_t*)(CLIC_ADDR_BASE+CLIC_INT_ATTR_OFFSET+source*4);
}

void eclic_set_cliccfg (uint8_t cliccfg){
  *(volatile uint8_t*)(CLIC_ADDR_BASE+CLIC_CFG_OFFSET) = cliccfg;
}

uint8_t eclic_get_cliccfg  (){
  return *(volatile uint8_t*)(CLIC_ADDR_BASE+CLIC_CFG_OFFSET);
}

void eclic_set_mth (uint8_t mth){
  *(volatile uint8_t*)(CLIC_ADDR_BASE+CLIC_MTH_OFFSET) = mth;
}

uint8_t eclic_get_mth  (){
  return *(volatile uint8_t*)(CLIC_ADDR_BASE+CLIC_MTH_OFFSET);
}

//sets nlbits 
void eclic_set_nlbits(uint8_t nlbits) {
  //shift nlbits to correct position
  uint8_t nlbits_shifted = nlbits << CLIC_CFG_NLBITS_LSB;

  //read the current cliccfg 
  uint8_t old_cliccfg = eclic_get_cliccfg();
  uint8_t new_cliccfg = (old_cliccfg & (~CLIC_CFG_NLBITS_MASK)) | (CLIC_CFG_NLBITS_MASK & nlbits_shifted); 

  eclic_set_cliccfg(new_cliccfg);
}

//get nlbits 
uint8_t eclic_get_nlbits() {
  //extract nlbits
  uint8_t nlbits = eclic_get_cliccfg();
  nlbits = (nlbits & CLIC_CFG_NLBITS_MASK) >> CLIC_CFG_NLBITS_LSB;
}

//sets an interrupt level based encoding of nlbits and CLICINTCTLBITS
uint8_t eclic_set_int_level(uint32_t source, uint8_t level) {
  //extract nlbits
  uint8_t nlbits = eclic_get_nlbits();
  if (nlbits > CLICINTCTLBITS) {
    nlbits = CLICINTCTLBITS; 
  }

  //shift level right to mask off unused bits
  level = level >> (8-nlbits);
  //shift level into correct bit position
  level = level << (8-nlbits);
 
  //write to clicintctrl
  uint8_t current_intctrl = eclic_get_intctrl(source);
  //shift intctrl left to mask off unused bits
  current_intctrl = current_intctrl << nlbits;
  //shift intctrl into correct bit position
  current_intctrl = current_intctrl >> nlbits;

  eclic_set_intctrl(source, (current_intctrl | level));

  return level;
}

//gets an interrupt level based encoding of nlbits
uint8_t eclic_get_int_level(uint32_t source) {
  //extract nlbits
  uint8_t nlbits = eclic_get_nlbits();
  if (nlbits > CLICINTCTLBITS) {
    nlbits = CLICINTCTLBITS; 
  }

  uint8_t intctrl = eclic_get_intctrl(source);

  //shift intctrl
  intctrl = intctrl >> (8-nlbits);
  //shift intctrl
  uint8_t level = intctrl << (8-nlbits);

  return level;
}

void eclic_mode_enable() {
  uint32_t mtvec_value = read_csr(mtvec);
  mtvec_value = mtvec_value & 0xFFFFFFC0;
  mtvec_value = mtvec_value | 0x00000003;
  write_csr(mtvec,mtvec_value);
}


