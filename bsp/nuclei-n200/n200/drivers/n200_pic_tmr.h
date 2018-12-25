// See LICENSE file for licence details

#ifndef N200_PIC_TMR_H
#define N200_PIC_TMR_H

  // Need to know the following info from the soc.h, so include soc.h here
  //   SOC_PIC_CTRL_ADDR      : what is the base address of PIC in this SoC
  //   SOC_PIC_NUM_INTERRUPTS : how much of irq configured in total for the PIC in this SoC
  //   SOC_TMR_CTRL_ADDR      : what is the base address of TMR in this SoC 
  //   SOC_TMR_FREQ           : what is the frequency for TMR to count (by rtc_toggle_a signal)
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

#define TMR_MSIP 0x0000
#define TMR_MSIP_size   0x4
#define TMR_MTIMECMP 0x0020
#define TMR_MTIMECMP_size 0x8
#define TMR_MTIME 0x0010
#define TMR_MTIME_size 0x8

#define TMR_CTRL_ADDR           SOC_TMR_CTRL_ADDR
#define TMR_REG(offset)         _REG32(TMR_CTRL_ADDR, offset)
#define TMR_FREQ   SOC_TMR_FREQ


#endif
