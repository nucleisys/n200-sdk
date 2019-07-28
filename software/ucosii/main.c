#include<ucos_ii.h>
#include<stdint.h>
#include<cpu.h>

#include "n200/drivers/n200_func.h"
#include "soc/drivers/soc.h"
#include "soc/drivers/board.h"
#include "n200/drivers/riscv_encoding.h"
#include "n200/drivers/n200_timer.h"
#include "n200/drivers/n200_eclic.h"


#define BUTTON_1_GPIO_OFFSET 30
#define BUTTON_2_GPIO_OFFSET 31

#define ECLIC_INT_DEVICE_BUTTON_1 (SOC_ECLIC_INT_GPIO_BASE + BUTTON_1_GPIO_OFFSET)
#define ECLIC_INT_DEVICE_BUTTON_2 (SOC_ECLIC_INT_GPIO_BASE + BUTTON_2_GPIO_OFFSET)
// The real value is:
//#define ECLIC_INT_DEVICE_BUTTON_1 49 // 30+19
//#define ECLIC_INT_DEVICE_BUTTON_2 50 // 31+19

// Since the BUTTON_1 ECLIC IRQ number is 49, and BUTTON_2 is 50, we need to overriede the irq49/50 handler 
#define BUTTON_1_HANDLER eclic_irq49_handler
#define BUTTON_2_HANDLER eclic_irq50_handler


#define mainQUEUE_LENGTH                    ( 1 )
static void prvSetupHardware( void );
extern void idle_task(void);


void wait_seconds(size_t n)
{
  unsigned long start_mtime, delta_mtime;

  // Don't start measuruing until we see an mtime tick
  unsigned long tmp = mtime_lo();
  do {
    start_mtime = mtime_lo();
  } while (start_mtime == tmp);

  do {
    delta_mtime = mtime_lo() - start_mtime;
  } while (delta_mtime < (n * TIMER_FREQ));

 // printf("-----------------Waited %d seconds.\n", n);
}


void config_eclic_irqs (){

  eclic_enable_interrupt (ECLIC_INT_DEVICE_BUTTON_1);
  eclic_enable_interrupt (ECLIC_INT_DEVICE_BUTTON_2);

  eclic_set_nlbits(3);
  //  The button have higher level

  eclic_set_irq_lvl_abs(ECLIC_INT_DEVICE_BUTTON_1, 2);
  eclic_set_irq_lvl_abs(ECLIC_INT_DEVICE_BUTTON_2, 3);

  //  The MTIME using Vector-Mode
  eclic_set_vmode(ECLIC_INT_MTIP);

 } 


void gpio_init(){
  GPIO_REG(GPIO_INPUT_EN)    &= ~((0x1<< RED_LED_GPIO_OFFSET) | (0x1<< GREEN_LED_GPIO_OFFSET) | (0x1 << BLUE_LED_GPIO_OFFSET)) ;
  GPIO_REG(GPIO_OUTPUT_EN)   |=  ((0x1<< RED_LED_GPIO_OFFSET)| (0x1<< GREEN_LED_GPIO_OFFSET) | (0x1 << BLUE_LED_GPIO_OFFSET)) ;

  GPIO_REG(GPIO_OUTPUT_VAL)  |=   (0x1 << RED_LED_GPIO_OFFSET) ;
  GPIO_REG(GPIO_OUTPUT_VAL)  &=  ~((0x1<< BLUE_LED_GPIO_OFFSET) | (0x1<< GREEN_LED_GPIO_OFFSET)) ;
}


void button_init(){

    GPIO_REG(GPIO_OUTPUT_EN)  &= ~((0x1 << BUTTON_1_GPIO_OFFSET) | (0x1 << BUTTON_2_GPIO_OFFSET));
    GPIO_REG(GPIO_PULLUP_EN)  &= ~((0x1 << BUTTON_1_GPIO_OFFSET) | (0x1 << BUTTON_2_GPIO_OFFSET));
    GPIO_REG(GPIO_INPUT_EN)   |=  ((0x1 << BUTTON_1_GPIO_OFFSET) | (0x1 << BUTTON_2_GPIO_OFFSET));
    
    GPIO_REG(GPIO_RISE_IE) |= ((0x1 << BUTTON_1_GPIO_OFFSET) | (0x1 << BUTTON_2_GPIO_OFFSET));
    GPIO_REG(GPIO_FALL_IE) &= ~((0x1 << BUTTON_1_GPIO_OFFSET) | (0x1 << BUTTON_2_GPIO_OFFSET));
    GPIO_REG(GPIO_HIGH_IE) &= ~((0x1 << BUTTON_1_GPIO_OFFSET) | (0x1 << BUTTON_2_GPIO_OFFSET));
    GPIO_REG(GPIO_LOW_IE) &= ~((0x1 << BUTTON_1_GPIO_OFFSET) | (0x1 << BUTTON_2_GPIO_OFFSET));

}


void BUTTON_1_HANDLER(void) {   

    printf ("%s","----Begin button1 handler\n");
    GPIO_REG(GPIO_OUTPUT_VAL) ^= (0x1 << BLUE_LED_GPIO_OFFSET);
    printf ("%s","----red LED off or on\n");
    wait_seconds(5);
    printf ("%s","----End button1 handler\n");

    GPIO_REG(GPIO_RISE_IP) = (0x1 << BUTTON_1_GPIO_OFFSET);  
};
void BUTTON_2_HANDLER(void) {
 
    printf ("%s","--------Begin button2 handler\n");
    printf ("%s","--------Higher level\n");
    GPIO_REG(GPIO_OUTPUT_VAL) ^= (0x1 << GREEN_LED_GPIO_OFFSET);
    wait_seconds(5);
    printf ("%s","--------End button2 handler\n");    

    GPIO_REG(GPIO_RISE_IP) = (0x1 << BUTTON_2_GPIO_OFFSET);

};


void prvSetupHardware( void )
{
    button_init();
    gpio_init();
    config_eclic_irqs();
}


#define STK_LEN 1024 

OS_STK task1_stk[STK_LEN];
OS_STK task2_stk[STK_LEN];
OS_STK task3_stk[STK_LEN];
OS_STK start_stk[STK_LEN];

#define TASK1_PRIO       			13 
#define TASK2_PRIO       			12 
#define TASK3_PRIO       			11
#define TASK_START_PRIO       			10 


void task1(void *args)
{
	int cnt=0;
	for(;;)
	{
		cnt++;
		printf("task1 is runing... %d\n",cnt);
     GPIO_REG(GPIO_OUTPUT_VAL) ^= (0x1 << BLUE_LED_GPIO_OFFSET);
		OSTimeDly(200);
	}
}

void task2(void *args)
{
	int cnt=0;
	for(;;)
	{
		cnt++;
		printf("task2 is runing... %d\n",cnt);
     GPIO_REG(GPIO_OUTPUT_VAL) ^= (0x1 << GREEN_LED_GPIO_OFFSET);
		OSTimeDly(100);
	}
}

void task3(void *args)
{
	int cnt=0;
	for(;;)
	{
		cnt++;
		printf("task3 is runing... %d\n",cnt);
		OSTimeDly(100);
	}
}

void start_task(void *args)
{

	printf("start all task...\n");
  OSTaskCreate(task1,NULL,&task1_stk[STK_LEN-1],TASK1_PRIO);
	OSTaskCreate(task2,NULL,&task2_stk[STK_LEN-1],TASK2_PRIO);
  OSTaskCreate(task3,NULL,&task3_stk[STK_LEN-1],TASK3_PRIO);
  OSTaskSuspend(TASK_START_PRIO);
//	enable_time_int();
//	enable_global_int();
 //for(;;){}
}

int main(void)
{
	printf("Start ucosii...\n");
	prvSetupHardware();
	OSInit();
	OSTaskCreate(start_task,NULL,&start_stk[STK_LEN-1],TASK_START_PRIO);
	printf("create start task success \r\n");
	OSStart();
	while(1){}
}


