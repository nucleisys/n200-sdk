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
#include "n22/drivers/clic_driver.h"

#define BUTTON_1_GPIO_OFFSET 30
#define BUTTON_2_GPIO_OFFSET 31

#define CLIC_HART0_ADDR              SOC_CLIC_CTRL_ADDR
#define CLIC_INT_DEVICE_BUTTON_1     (SOC_CLIC_INT_GPIO_BASE + 19 + BUTTON_1_GPIO_OFFSET)
#define CLIC_INT_DEVICE_BUTTON_2     (SOC_CLIC_INT_GPIO_BASE + 19 + BUTTON_2_GPIO_OFFSET)

//clic data structure
clic_instance_t clic;

/**************************************************************/
void default_handler(void)
{
   uint32_t source = read_csr(mcause) & 0x3ff;
   printf("default handler(%d)\n", source);

   // Clear pending bit
   clic_clear_pending(&clic, source);
}

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
void handle_m_time_interrupt(){

  printf("%s", "Begin mtime handler\n");
  GPIO_REG(GPIO_OUTPUT_VAL) ^= (0x1 << RED_LED_GPIO_OFFSET);

  volatile uint64_t * mtime       = (uint64_t*) (TMR_CTRL_ADDR + TMR_MTIME);
  volatile uint64_t * mtimecmp    = (uint64_t*) (TMR_CTRL_ADDR + TMR_MTIMECMP);
  uint64_t now = *mtime;
  uint64_t then = now + 0.5 * TMR_FREQ;// Here we set 1 second, but we need to wait 5 cycles to get out from this handler, so the blink will not toggle as 1 cycle
  *mtimecmp = then;

  wait_seconds(5);// Wait for a while
  
  printf("%s","End mtime handler\n");

  clic_clear_pending(&clic, IRQ_M_TIMER);
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


const char * print_instructions_msg= " \
\n\
\n\
\n\
\n\
This is print function printed:  \n\
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




void button_1_handler(void) {

  printf("%s", "----Begin button1 handler\n");

  // Green LED On
  GPIO_REG(GPIO_OUTPUT_VAL) |= (1 << GREEN_LED_GPIO_OFFSET);

  // Clear the GPIO Pending interrupt by writing 1.
  GPIO_REG(GPIO_RISE_IP) = (0x1 << BUTTON_1_GPIO_OFFSET);

  wait_seconds(5);// Wait for a while

  printf("%s","----End button1 handler\n");

  clic_clear_pending(&clic, CLIC_INT_DEVICE_BUTTON_1);
};


void button_2_handler(void) {

  printf("%s", "--------Begin button2 handler\n");

  // Blue LED On
  GPIO_REG(GPIO_OUTPUT_VAL) |= (1 << BLUE_LED_GPIO_OFFSET);

  GPIO_REG(GPIO_RISE_IP) = (0x1 << BUTTON_2_GPIO_OFFSET);

  wait_seconds(5);// Wait for a while

  printf("%s","--------End button2 handler\n");

  clic_clear_pending(&clic, CLIC_INT_DEVICE_BUTTON_2);
};

void clic_isr_setup(uint32_t source, interrupt_function_ptr_t isr, uint8_t level){

  clic_install_handler(&clic, source, isr);
  clic_set_int_level(&clic, source, level);
  //clic_set_attribute(&clic, source, CLIC_INT_ATTR_TRIG_NEG);
  clic_enable_interrupt(&clic, source);
}

void clic_setup (){
  clic_init(&clic, CLIC_HART0_ADDR, localISR, default_handler);

  // Use all bits for levels, no shv
  clic_set_cliccfg(&clic, (clic.num_config_bits << 1));
  clic_set_clicthresh (&clic, 0);
  
  // Setup user's isr
  clic_isr_setup(IRQ_M_TIMER, handle_m_time_interrupt, 1);
  clic_isr_setup(CLIC_INT_DEVICE_BUTTON_1, button_1_handler, 2);
  clic_isr_setup(CLIC_INT_DEVICE_BUTTON_2, button_2_handler, 3);
} 


void setup_mtime (){

    // Set the machine timer to go off in 3 seconds.
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
  printf("%s", print_instructions_msg);

  printf("%s","\nPlease enter any letter from keyboard to continue!\n");

  char c;
  // Check for user input
  while(1){
    if (_getc(&c) != 0){
       printf("%s","I got an input, it is\n\r");
       break;
    }
  }
  _putc(c);
  printf("%s","\n\nThank you for supporting RISC-V, you will see the blink soon on the board!\n");

  clic_setup();

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
