// See LICENSE for license details.

#include <stdint.h>
#include "clic_driver.h"
#include "riscv_encoding.h"
#include <string.h>

#include <stdio.h>



// Note that there are no assertions or bounds checking on these
// parameter values.
void clic_init (
                clic_instance_t * this_clic,
                uintptr_t hart_addr,
                interrupt_function_ptr_t* vect_table,
                interrupt_function_ptr_t default_handler
                )
{
    this_clic->hart_addr =  hart_addr;
    this_clic->vect_table = vect_table;

    uint32_t clic_info = *(volatile uint32_t*)(this_clic->hart_addr + CLIC_INFO_OFFSET);
    this_clic->num_config_bits = (clic_info >> 21) & 0xf;
    this_clic->num_sources = clic_info & 0x1fff;
    
    //initialize vector table
    for(int i = 0; i < this_clic->num_sources; i++)  {
      this_clic->vect_table[i] = default_handler;
    }
  
    //set csr mtvt
    write_csr(0x307, vect_table);
  
    //clear all interrupt enables and pending
    for(int i = 0; i < this_clic->num_sources; i++)  {
      *(volatile uint8_t*)(this_clic->hart_addr + CLIC_INT_IE_OFFSET + 4 * i) = 0;
      *(volatile uint8_t*)(this_clic->hart_addr + CLIC_INT_IP_OFFSET + 4 * i) = 0;
    }
  
    //clear cfg register
    *(volatile uint8_t*)(this_clic->hart_addr + CLIC_CFG_OFFSET) = 0;

    //clear minthresh register
    *(volatile uint8_t*)(this_clic->hart_addr + CLIC_MTH_OFFSET) = 0;
}

void clic_install_handler (clic_instance_t * this_clic, uint32_t source, interrupt_function_ptr_t handler) {
    this_clic->vect_table[source] = handler;
}

void clic_enable_interrupt (clic_instance_t * this_clic, uint32_t source) {
    *(volatile uint8_t*)(this_clic->hart_addr + CLIC_INT_IE_OFFSET + 4 * source) = 1;
}

void clic_disable_interrupt (clic_instance_t * this_clic, uint32_t source){
    *(volatile uint8_t*)(this_clic->hart_addr + CLIC_INT_IE_OFFSET + 4 * source) = 0;
}

void clic_set_pending(clic_instance_t * this_clic, uint32_t source){
    *(volatile uint8_t*)(this_clic->hart_addr + CLIC_INT_IP_OFFSET + 4 * source) = 1;
}

void clic_clear_pending(clic_instance_t * this_clic, uint32_t source){
    *(volatile uint8_t*)(this_clic->hart_addr + CLIC_INT_IP_OFFSET + 4 * source) = 0;
}

uint8_t clic_set_attribute( clic_instance_t * this_clic, uint32_t source, uint8_t attr)
{
    *(volatile uint8_t*)(this_clic->hart_addr + CLIC_INT_ATTR_OFFSET + 4 * source) = attr;
}

uint8_t clic_get_attribute( clic_instance_t * this_clic, uint32_t source)
{
    return *(volatile uint8_t*)(this_clic->hart_addr + CLIC_INT_ATTR_OFFSET + 4 * source);
}

void clic_set_intctl (clic_instance_t * this_clic, uint32_t source, uint32_t intctl){
    *(volatile uint8_t*)(this_clic->hart_addr + CLIC_INT_CTRL_OFFSET + 4 * source) = intctl;
}

uint8_t clic_get_intctl  (clic_instance_t * this_clic, uint32_t source){
    return *(volatile uint8_t*)(this_clic->hart_addr + CLIC_INT_CTRL_OFFSET + 4 * source);
}

void clic_set_cliccfg (clic_instance_t * this_clic, uint32_t cfg){
    *(volatile uint8_t*)(this_clic->hart_addr + CLIC_CFG_OFFSET) = cfg;
}

uint8_t clic_get_cliccfg  (clic_instance_t * this_clic){
    return *(volatile uint8_t*)(this_clic->hart_addr + CLIC_CFG_OFFSET);
}

void clic_set_clicthresh (clic_instance_t * this_clic, uint32_t thresh) {
    *(volatile uint8_t*)(this_clic->hart_addr + CLIC_MTH_OFFSET) = thresh;
}

uint8_t clic_get_clicthresh  (clic_instance_t * this_clic) {
    return *(volatile uint8_t*)(this_clic->hart_addr + CLIC_MTH_OFFSET);
}

//sets an interrupt level based encoding of nmbits, nlbits
uint8_t clic_set_int_level( clic_instance_t * this_clic, uint32_t source, uint8_t level) {
    //extract nlbits
    uint8_t nlbits = clic_get_cliccfg(this_clic);
    nlbits = (nlbits >>1) & 0xf;
  
    //shift level right to mask off unused bits
    level = level>>((this_clic->num_config_bits)-nlbits); //plus this_clic->nmbits which is always 0 for now.
    //shift level into correct bit position
    level = level << (8-this_clic->num_config_bits) + (this_clic->num_config_bits - nlbits);
   
    //write to clicintctl
    uint8_t current_intctl = clic_get_intctl(this_clic, source);
    clic_set_intctl(this_clic, source, (current_intctl | level));
  
    return level;
}

//gets an interrupt level based encoding of nmbits, nlbits
uint8_t clic_get_int_level( clic_instance_t * this_clic, uint32_t source) {
    uint8_t level;
    level = clic_get_intctl(this_clic, source);
  
    //extract nlbits
    uint8_t nlbits = clic_get_cliccfg(this_clic);
    nlbits = (nlbits >>1) & 0xf;
  
    //shift level
    level = level >> (8-(this_clic->num_config_bits));
  
    //shift level right to mask off priority bits
    level = level>>(this_clic->num_config_bits-nlbits); //this_clic->nmbits which is always 0 for now.
  
    return level;
}

//sets an interrupt priority based encoding of nmbits, nlbits
uint8_t clic_set_int_priority( clic_instance_t * this_clic, uint32_t source, uint8_t priority) {
    //priority bits = num_config_bits - nlbits
    //extract nlbits
    uint8_t nlbits = clic_get_cliccfg(this_clic);
    nlbits = (nlbits >>1) & 0xf;
  
    uint8_t priority_bits = this_clic->num_config_bits-nlbits;
    if(priority_bits = 0) {
      //no bits to set
      return 0;
    }
    //mask off unused bits
    priority = priority >> (8-priority_bits);
    //shift into the correct bit position
    priority = priority << (8-(this_clic->num_config_bits));
  
    //write to clicintctl
    uint8_t current_intctl = clic_get_intctl(this_clic, source);
    clic_set_intctl(this_clic, source, (current_intctl | priority));

    return current_intctl;
}

//gets an interrupt priority based encoding of nmbits, nlbits
uint8_t clic_get_int_priority( clic_instance_t * this_clic, uint32_t source) {
    uint8_t priority;
    priority = clic_get_intctl(this_clic, source);
  
    //extract nlbits
    uint8_t nlbits = clic_get_cliccfg(this_clic);
    nlbits = (nlbits >>1) & 0xf;
  
    //shift left to mask off level bits
    priority = priority << nlbits;
  
    //shift priority
    priority = priority >> (8-((this_clic->num_config_bits)+nlbits));
  
    return priority;
}

