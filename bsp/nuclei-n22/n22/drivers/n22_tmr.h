// See LICENSE file for licence details

#ifndef N22_TMR_H
#define N22_TMR_H

  // Need to know the following info from the soc.h, so include soc.h here
  //   SOC_TMR_CTRL_ADDR      : what is the base address of TMR in this SoC 
  //   SOC_TMR_FREQ           : what is the frequency for TMR to count (by rtc_toggle_a signal)
#include "soc/drivers/soc.h"


#define TMR_MSIP 0xFFC
#define TMR_MSIP_size   0x4
#define TMR_MTIMECMP 0x8
#define TMR_MTIMECMP_size 0x8
#define TMR_MTIME 0x0
#define TMR_MTIME_size 0x8

#define TMR_CTRL_ADDR           SOC_TMR_CTRL_ADDR
#define TMR_REG(offset)         _REG32(TMR_CTRL_ADDR, offset)
#define TMR_FREQ   SOC_TMR_FREQ


#endif
