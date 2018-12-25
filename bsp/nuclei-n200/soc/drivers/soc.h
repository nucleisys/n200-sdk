// See LICENSE for license details.

#ifndef _NUCLEI_SOC_H
#define _NUCLEI_SOC_H

#include "n200/drivers/riscv_const.h"
#include "soc/drivers/aon.h"
#include "soc/drivers/gpio.h"
#include "soc/drivers/pwm.h"
#include "soc/drivers/spi.h"
#include "soc/drivers/uart.h"
#include "soc/drivers/i2c.h"

#include "soc/drivers/board.h"


/****************************************************************************
 * Platform definitions
 *****************************************************************************/

#define TRAPVEC_TABLE_CTRL_ADDR _AC(0x00001010,UL)

#define SOC_PIC_CTRL_ADDR       _AC(0x0C000000,UL)
#define SOC_TMR_CTRL_ADDR       _AC(0x02000000,UL)
#define AON_CTRL_ADDR           _AC(0x10000000,UL)
#define GPIO_CTRL_ADDR          _AC(0x10012000,UL)
#define UART0_CTRL_ADDR         _AC(0x10013000,UL)
#define SPI0_CTRL_ADDR          _AC(0x10014000,UL)
#define PWM0_CTRL_ADDR          _AC(0x10015000,UL)
#define UART1_CTRL_ADDR         _AC(0x10023000,UL)
#define SPI1_CTRL_ADDR          _AC(0x10024000,UL)
#define PWM1_CTRL_ADDR          _AC(0x10025000,UL)
#define SPI2_CTRL_ADDR          _AC(0x10034000,UL)
#define PWM2_CTRL_ADDR          _AC(0x10035000,UL)
#define I2C_CTRL_ADDR           _AC(0x10042000,UL)
#define SPI0_MMAP_ADDR          _AC(0x20000000,UL)
#define MEM_CTRL_ADDR           _AC(0x80000000,UL)

// IOF Mappings
#define IOF0_SPI1_MASK          _AC(0x000007FC,UL)
#define SPI11_NUM_SS     (4)
#define IOF_SPI1_SS0          (2u)
#define IOF_SPI1_SS1          (8u)
#define IOF_SPI1_SS2          (9u)
#define IOF_SPI1_SS3          (10u)
#define IOF_SPI1_MOSI         (3u)
#define IOF_SPI1_MISO         (4u)
#define IOF_SPI1_SCK          (5u)
#define IOF_SPI1_DQ0          (3u)
#define IOF_SPI1_DQ1          (4u)
#define IOF_SPI1_DQ2          (6u)
#define IOF_SPI1_DQ3          (7u)

#define IOF0_SPI2_MASK          _AC(0xFC000000,UL)
#define SPI2_NUM_SS       (1)
#define IOF_SPI2_SS0          (26u)
#define IOF_SPI2_MOSI         (27u)
#define IOF_SPI2_MISO         (28u)
#define IOF_SPI2_SCK          (29u)
#define IOF_SPI2_DQ0          (27u)
#define IOF_SPI2_DQ1          (28u)
#define IOF_SPI2_DQ2          (30u)
#define IOF_SPI2_DQ3          (31u)

#define IOF0_UART0_MASK         _AC(0x00030000, UL)
#define IOF_UART0_RX   (16u)
#define IOF_UART0_TX   (17u)

#define IOF0_UART1_MASK         _AC(0x03000000, UL)
#define IOF_UART1_RX (24u)
#define IOF_UART1_TX (25u)

#define IOF0_I2C_MASK           _AC(0x00003000, UL)
#define IOF_I2C_SDA (12u)
#define IOF_I2C_SCL (13u)

#define IOF1_PWM0_MASK          _AC(0x0000000F, UL)
#define IOF1_PWM1_MASK          _AC(0x00780000, UL)
#define IOF1_PWM2_MASK          _AC(0x00003C00, UL)

// Interrupt Numbers
#define SOC_PIC_NUM_INTERRUPTS 54


#define SOC_PIC_INT_WDOGCMP     3
#define SOC_PIC_INT_RTCCMP      4
#define SOC_PIC_INT_UART0_BASE  5
#define SOC_PIC_INT_UART1_BASE  6
#define SOC_PIC_INT_SPI0_BASE   7
#define SOC_PIC_INT_SPI1_BASE   8
#define SOC_PIC_INT_SPI2_BASE   9
#define SOC_PIC_INT_GPIO_BASE   10

#define SOC_PIC_INT_PWM0_BASE   42
#define SOC_PIC_INT_PWM1_BASE   46
#define SOC_PIC_INT_PWM2_BASE   50
#define SOC_PIC_INT_I2C_BASE    54

// The TMR frequency is just the RTC frequency
#define SOC_TMR_FREQ RTC_FREQ

// Helper functions
#define _REG8(p, i)             (*(volatile uint8_t *) ((p) + (i)))
#define _REG32(p, i)            (*(volatile uint32_t *) ((p) + (i)))
#define _REG32P(p, i)           ((volatile uint32_t *) ((p) + (i)))
#define AON_REG(offset)         _REG32(AON_CTRL_ADDR, offset)
#define GPIO_REG(offset)        _REG32(GPIO_CTRL_ADDR, offset)
#define OTP_REG(offset)         _REG32(OTP_CTRL_ADDR, offset)
#define PRCI_REG(offset)        _REG32(PRCI_CTRL_ADDR, offset)
#define PWM0_REG(offset)        _REG32(PWM0_CTRL_ADDR, offset)
#define PWM1_REG(offset)        _REG32(PWM1_CTRL_ADDR, offset)
#define PWM2_REG(offset)        _REG32(PWM2_CTRL_ADDR, offset)
#define SPI0_REG(offset)        _REG32(SPI0_CTRL_ADDR, offset)
#define SPI1_REG(offset)        _REG32(SPI1_CTRL_ADDR, offset)
#define SPI2_REG(offset)        _REG32(SPI2_CTRL_ADDR, offset)
#define UART0_REG(offset)       _REG32(UART0_CTRL_ADDR, offset)
#define UART1_REG(offset)       _REG32(UART1_CTRL_ADDR, offset)
#define I2C_REG(offset)         _REG8(I2C_CTRL_ADDR, offset)

// Misc

#define NUM_GPIO 32



#endif /* _NUCLEI_SOC_H */
