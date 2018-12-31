// See LICENSE file for licence details

#ifndef N22_FUNC_H
#define N22_FUNC_H

__BEGIN_DECLS

#include "n22/drivers/n22_tmr.h"
#include "n22/drivers/n22_clic.h"

void pmp_open_all_space();

void switch_m2u_mode();

uint32_t get_tmr_freq();

uint32_t mtime_lo(void);

uint32_t mtime_hi(void);

uint64_t get_mtime_value();

uint64_t get_instret_value();

uint64_t get_cycle_value();

uint32_t get_cpu_freq();

uint32_t __attribute__((noinline)) measure_cpu_freq(size_t n);


///////////////////////////////////////////////////////////////////
/////// CLIC relevant functions
///////
void clic_init ( uint32_t num_irq );

void clic_enable_interrupt (uint32_t source);
void clic_disable_interrupt (uint32_t source);

void clic_set_pending(uint32_t source);
void clic_clear_pending(uint32_t source);

void clic_set_intctrl (uint32_t source, uint8_t intctrl);
uint8_t clic_get_intctrl  (uint32_t source);

void clic_set_intattr (uint32_t source, uint8_t intattr);
uint8_t clic_get_intattr  (uint32_t source);

void clic_set_cliccfg (uint8_t cliccfg);
uint8_t clic_get_cliccfg ();

void clic_set_mth (uint8_t mth);
uint8_t clic_get_mth();

//sets nlbits 
void clic_set_nlbits(uint8_t nlbits);


//get nlbits 
uint8_t clic_get_nlbits();

uint8_t clic_set_int_level(uint32_t source, uint8_t level);
uint8_t clic_get_int_level(uint32_t source);

void clic_mode_enable();

__END_DECLS

#endif
