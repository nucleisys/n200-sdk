// See LICENSE file for licence details

#ifndef N200_FUNC_H
#define N200_FUNC_H

__BEGIN_DECLS

#include "n200/drivers/n200_pic_tmr.h"

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
/////// PIC relevant functions
///////
void pic_init (
                uintptr_t base_addr,
                uint32_t num_sources,
                uint32_t num_priorities
                );

void pic_set_threshold (
			 uint32_t threshold);
  
void pic_enable_interrupt (
			    uint32_t source);

void pic_disable_interrupt (
			     uint32_t source);
  
void pic_set_priority (
			uint32_t source,
			uint32_t priority);

uint32_t pic_claim_interrupt();

void pic_complete_interrupt(
			     uint32_t source);

uint32_t pic_check_eip();

// Structures for registering different interrupt handlers
// for different parts of the application.
typedef void (*function_ptr_t) (void);

// The interrupt 0 is empty
__attribute__((weak)) function_ptr_t pic_interrupt_handlers[PIC_NUM_INTERRUPTS];


__END_DECLS

#endif
