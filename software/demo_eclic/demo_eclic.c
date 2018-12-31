// See LICENSE for license details.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "stdatomic.h"

#include "n22/drivers/n22_func.h"
#include "soc/drivers/soc.h"
#include "soc/drivers/board.h"
#include "n22/drivers/riscv_encoding.h"
#include "n22/drivers/n22_tmr.h"
#include "n22/drivers/n22_clic.h"

#define BUTTON_1_GPIO_OFFSET 30
#define BUTTON_2_GPIO_OFFSET 31

   //SOC_CLIC_INT_GPIO_BASE   7
   // #define CLIC_INT_DEVICE_BUTTON_1 (SOC_CLIC_INT_GPIO_BASE + BUTTON_1_GPIO_OFFSET)
   // #define CLIC_INT_DEVICE_BUTTON_2 (SOC_CLIC_INT_GPIO_BASE + BUTTON_2_GPIO_OFFSET)
#define CLIC_INT_DEVICE_BUTTON_1 37
#define CLIC_INT_DEVICE_BUTTON_2 38

#define BUTTON_1_HANDLER clic_irq37_handler
#define BUTTON_2_HANDLER clic_irq38_handler


void reset_demo (void);



void no_interrupt_handler (void) {};

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
  } while (delta_mtime < (n * TMR_FREQ));

  printf("-----------------Waited %d seconds.\n", n);
}


/*Entry Point for Machine Timer Interrupt Handler*/
void MTIME_HANDLER(){

  printf ("%s","Begin mtime handler\n");
  GPIO_REG(GPIO_OUTPUT_VAL) ^= (0x1 << RED_LED_GPIO_OFFSET);

  volatile uint64_t * mtime       = (uint64_t*) (TMR_CTRL_ADDR + TMR_MTIME);
  volatile uint64_t * mtimecmp    = (uint64_t*) (TMR_CTRL_ADDR + TMR_MTIMECMP);
  uint64_t now = *mtime;
  uint64_t then = now + 0.5 * TMR_FREQ;// Here we set 1 second, but we need to wait 5 cycles to get out from this handler, so the blink will not toggle as 1 cycle
  *mtimecmp = then;

  wait_seconds(5);// Wait for a while
  
  printf ("%s","End mtime handler\n");

}



static void _putc(char c) {
  while ((int32_t) UART0_REG(UART_REG_TXFIFO) < 0);
  UART0_REG(UART_REG_TXFIFO) = c;
}

int _getc(char * c){
  int32_t val = (int32_t) UART0_REG(UART_REG_RXFIFO);
  if (val > 0) {
    *c =  val & 0xFF;
    return 1;
  }
  return 0;
}

char * read_instructions_msg= " \
\n\
 ";


const char * printf_instructions_msg= " \
\n\
\n\
\n\
\n\
This is printf function printed:  \n\
\n\
             !! Here We Go, HummingBird !! \n\
\n\
     ######    ###    #####   #####          #     #\n\
     #     #    #    #     # #     #         #     #\n\
     #     #    #    #       #               #     #\n\
     ######     #     #####  #        #####  #     #\n\
     #   #      #          # #                #   #\n\
     #    #     #    #     # #     #           # #\n\
     #     #   ###    #####   #####             #\n\
\n\
 ";




void BUTTON_1_HANDLER(void) {

  printf ("%s","----Begin button1 handler\n");

  // Green LED On
  GPIO_REG(GPIO_OUTPUT_VAL) |= (1 << GREEN_LED_GPIO_OFFSET);

  // Clear the GPIO Pending interrupt by writing 1.
  GPIO_REG(GPIO_RISE_IP) = (0x1 << BUTTON_1_GPIO_OFFSET);

  wait_seconds(5);// Wait for a while

  printf ("%s","----End button1 handler\n");
};


void BUTTON_2_HANDLER(void) {

  printf ("%s","--------Begin button2 handler\n");

  // Blue LED On
  GPIO_REG(GPIO_OUTPUT_VAL) |= (1 << BLUE_LED_GPIO_OFFSET);

  GPIO_REG(GPIO_RISE_IP) = (0x1 << BUTTON_2_GPIO_OFFSET);

  wait_seconds(5);// Wait for a while

  printf ("%s","--------End button2 handler\n");

};

void config_clic_irqs (){

  // Have to enable the interrupt both at the GPIO level,
  // and at the CLIC level.
  clic_enable_interrupt (CLIC_INT_TMR);
  clic_enable_interrupt (CLIC_INT_DEVICE_BUTTON_1);
  clic_enable_interrupt (CLIC_INT_DEVICE_BUTTON_2);

  clic_set_nlbits(4);
  //  The button have higher level
  clic_set_int_level(CLIC_INT_TMR, 1);
  clic_set_int_level(CLIC_INT_DEVICE_BUTTON_1, 2);
  clic_set_int_level(CLIC_INT_DEVICE_BUTTON_2, 3);

 } 


void setup_mtime (){

    // Set the machine timer to go off in 2 seconds.
    volatile uint64_t * mtime    = (uint64_t*) (TMR_CTRL_ADDR + TMR_MTIME);
    volatile uint64_t * mtimecmp = (uint64_t*) (TMR_CTRL_ADDR + TMR_MTIMECMP);
    uint64_t now = *mtime;
    uint64_t then = now + 2 * TMR_FREQ;
    *mtimecmp = then;

}

int main(int argc, char **argv)
{
  // Set up the GPIOs such that the LED GPIO
  // can be used as both Inputs and Outputs.
  

  GPIO_REG(GPIO_OUTPUT_EN)  &= ~((0x1 << BUTTON_1_GPIO_OFFSET) | (0x1 << BUTTON_2_GPIO_OFFSET));
  GPIO_REG(GPIO_PULLUP_EN)  &= ~((0x1 << BUTTON_1_GPIO_OFFSET) | (0x1 << BUTTON_2_GPIO_OFFSET));
  GPIO_REG(GPIO_INPUT_EN)   |=  ((0x1 << BUTTON_1_GPIO_OFFSET) | (0x1 << BUTTON_2_GPIO_OFFSET));

  GPIO_REG(GPIO_RISE_IE) |= (1 << BUTTON_1_GPIO_OFFSET);
  GPIO_REG(GPIO_RISE_IE) |= (1 << BUTTON_2_GPIO_OFFSET);


  GPIO_REG(GPIO_INPUT_EN)    &= ~((0x1<< RED_LED_GPIO_OFFSET) | (0x1<< GREEN_LED_GPIO_OFFSET) | (0x1 << BLUE_LED_GPIO_OFFSET)) ;
  GPIO_REG(GPIO_OUTPUT_EN)   |=  ((0x1<< RED_LED_GPIO_OFFSET)| (0x1<< GREEN_LED_GPIO_OFFSET) | (0x1 << BLUE_LED_GPIO_OFFSET)) ;

  GPIO_REG(GPIO_OUTPUT_VAL)  |=   (0x1 << RED_LED_GPIO_OFFSET) ;
  GPIO_REG(GPIO_OUTPUT_VAL)  &=  ~((0x1<< BLUE_LED_GPIO_OFFSET) | (0x1<< GREEN_LED_GPIO_OFFSET)) ;


  
  // Print the message
  printf ("%s",printf_instructions_msg);

  //printf("\nIn main function, the mstatus is 0x%x\n", read_csr(mstatus));


  printf ("%s","\nPlease enter any letter from keyboard to continue!\n");

  char c;
  // Check for user input
  while(1){
    if (_getc(&c) != 0){
       printf ("%s","I got an input, it is\n\r");
       break;
    }
  }
  _putc(c);
  printf ("\n\r");
  printf ("%s","\nThank you for supporting RISC-V, you will see the blink soon on the board!\n");

  



  config_clic_irqs();

  setup_mtime();

  // Enable interrupts in general.
  set_csr(mstatus, MSTATUS_MIE);


  // For Bit-banging 
  
  uint32_t bitbang_mask = 0;
  bitbang_mask = (1 << 13);

  GPIO_REG(GPIO_OUTPUT_EN) |= bitbang_mask;

  
  while (1){
    GPIO_REG(GPIO_OUTPUT_VAL) ^= bitbang_mask;
    // For Bit-banging with Atomics demo if the A extension is supported.
    //atomic_fetch_xor_explicit(&GPIO_REG(GPIO_OUTPUT_VAL), bitbang_mask, memory_order_relaxed);
  }

  return 0;

}
