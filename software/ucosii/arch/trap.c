//#include<encoding.h>
#include<trap.h>
#include<cpu.h>
#include<os_cpu.h>
#include "ucos_ii.h"

#include "n200/drivers/n200_func.h"
#include "soc/drivers/soc.h"
#include "soc/drivers/board.h"
#include "n200/drivers/riscv_encoding.h"
#include "n200/drivers/n200_timer.h"
#include "n200/drivers/n200_eclic.h"




void syscall_handler(int32_t sp[])
{
	extern void task_switch(int32_t sp[]);
	task_switch(sp);

}

void time_handler(int32_t sp[])
{
extern void task_switch(int32_t sp[]);

  static uint64_t then = 0;
 
    volatile uint64_t * mtime       = (uint64_t*) (TIMER_CTRL_ADDR + TIMER_MTIME);
    volatile uint64_t * mtimecmp    = (uint64_t*) (TIMER_CTRL_ADDR + TIMER_MTIMECMP);
	
	if(then != 0)  {
		//next timer irq is 1 second from previous
		then += (RTC_CLOCK_HZ / TICK_RATE_HZ);
	} else{ //first time setting the timer
		uint64_t now = *mtime;
		then = now + (RTC_CLOCK_HZ /TICK_RATE_HZ);
	}
	*mtimecmp = then;

	OSIntEnter();
	OSTimeTick();
	OSIntExit(); //change OSIntExit , now it must be used with task_switch()
	task_switch(sp);
}


int32_t trap_handler(int32_t mcause,int32_t mepc,int32_t sp[],int32_t arg1)
{
		switch(mcause&0X00000fff)
		{
			case 11:
				 	sp[31]=mepc+4;
				  syscall_handler(sp);
				 mepc+=4;	
			  break;
			default:
				write(1, "trap\n", 5);
			  printf("In trap handler, the mcause is %d\n",(mcause&0X00000fff) );
       printf("In trap handler, the mepc is 0x%x\n", read_csr(mepc));
       printf("In trap handler, the mtval is 0x%x\n", read_csr(mbadaddr));
			  _exit(mcause);
				break;
		}
	return mepc;
}




void scheduler_ecall(void ){
		portYIELD();
}




void SetupTimer()	{

    uint8_t mtime_intattr;
    // Set the machine timer
    //Bob: update it to TMR
    //volatile uint64_t * mtime       = (uint64_t*) (CLINT_CTRL_ADDR + CLINT_MTIME);
    //volatile uint64_t * mtimecmp    = (uint64_t*) (CLINT_CTRL_ADDR + CLINT_MTIMECMP);
    volatile uint64_t * mtime       = (uint64_t*) (TIMER_CTRL_ADDR + TIMER_MTIME);
    volatile uint64_t * mtimecmp    = (uint64_t*) (TIMER_CTRL_ADDR + TIMER_MTIMECMP);
    uint64_t now = *mtime;
    uint64_t then = now + (RTC_CLOCK_HZ / TICK_RATE_HZ);
    *mtimecmp = then;

    mtime_intattr=eclic_get_intattr (ECLIC_INT_MTIP);
    mtime_intattr|=ECLIC_INT_ATTR_SHV;
    eclic_set_intattr(ECLIC_INT_MTIP,mtime_intattr);
    eclic_enable_interrupt (ECLIC_INT_MTIP);
		
    //eclic_set_nlbits(4);
    eclic_set_irq_lvl_abs(ECLIC_INT_MTIP,1);
    //set_csr(mstatus, MSTATUS_MIE);
}
/*-----------------------------------------------------------*/


void TickSetup()	{

	SetupTimer();
	OSIntNesting = 0;
}


int32_t timer_irq_handler(int32_t mcause,int32_t mepc,int32_t sp[])
{
	sp[31]=mepc;
	time_handler(sp);
}

void halt(int32_t mcause,int32_t mepc)
{
	extern void exit(int);
	disable_global_int();
	printf("halt ,mcause=%llx ,mepc=%llx\n",mcause,mepc);
	exit(0);
}


