#ifndef _TRAP_H
#define _TRAP_H

#include<cpu.h>



#ifndef NULL
#define NULL ((void*)0)
#endif


extern void halt(int32_t,int32_t);
extern void syscall_handler(int32_t sp[]);
extern int32_t trap_handler(int32_t mcause,int32_t mepc,int32_t sp[],int32_t arg1);
extern int32_t timer_irq_handler(int32_t mcause,int32_t mepc,int32_t sp[]);


/*-----------------------------------------------------------*/
/*System Calls												 */
/*-----------------------------------------------------------*/
//ecall macro used to store argument in a3
#define ECALL(arg) ({			\
	register uintptr_t a3 asm ("a3") = (uintptr_t)(arg);	\
	asm volatile ("ecall"					\
		      : "+r" (a3)				\
		      : 	\
		      : "memory");				\
	a3;							\
})

#define PORT_NO  		40

/*-----------------------------------------------------------*/


/* Scheduler utilities. */
/* the return after the ECALL is VERY important */

#define portYIELD() ECALL(PORT_NO);


#endif
