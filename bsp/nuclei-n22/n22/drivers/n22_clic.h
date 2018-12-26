// See LICENSE file for licence details

#ifndef N200_CLIC_H
#define N200_CLIC_H

  // Need to know the following info from the soc.h, so include soc.h here
  //   SOC_CLIC_CTRL_ADDR      : what is the base address of PIC in this SoC
  //   SOC_CLIC_NUM_INTERRUPTS : how much of irq configured in total for the PIC in this SoC
#include "soc/drivers/soc.h"


// 32 bits per source
#define PIC_PRIORITY_OFFSET            _AC(0x0000,UL)
#define PIC_PRIORITY_SHIFT_PER_SOURCE  2
// 1 bit per source (1 address)
#define PIC_PENDING_OFFSET             _AC(0x1000,UL)

//0x80 per target
#define PIC_ENABLE_OFFSET              0x1100
#define PIC_THRESHOLD_OFFSET           0x1200
#define PIC_CLAIM_OFFSET               0x1204
#define PIC_EIP_OFFSET                 0x1208

#define PIC_MAX_SOURCE                 1023
#define PIC_SOURCE_MASK                0x3FF

#define PIC_CTRL_ADDR           SOC_PIC_CTRL_ADDR
#define PIC_REG(offset)         _REG32(PIC_CTRL_ADDR, offset)
#define PIC_NUM_PRIORITIES 7

#define PIC_NUM_INTERRUPTS SOC_PIC_NUM_INTERRUPTS

#define PIC_INT_RESERVED    0
#define PIC_INT_SFT         1
#define PIC_INT_TMR         2

#endif
