// See LICENSE file for licence details

#ifndef N200_CLIC_H
#define N200_CLIC_H

  // Need to know the following info from the soc.h, so include soc.h here
  //   SOC_CLIC_CTRL_ADDR      : what is the base address of CLIC in this SoC
  //   SOC_CLIC_NUM_INTERRUPTS : how much of irq configured in total for the CLIC in this SoC
#include "soc/drivers/soc.h"

#define CLICINTCTLBITS  8

//CLIC memory map
//   Offset
//  0x0000       1B          RW        cliccfg
#define CLIC_CFG_OFFSET            0x0
//  0x0004       4B          R         clicinfo   
#define CLIC_INFO_OFFSET           0x4
//  0x000B       1B          RW        mintthresh 
#define CLIC_MTH_OFFSET         0xB
//
//  0x1000+4*i   1B/input    RW        clicintip[i]
#define CLIC_INT_IP_OFFSET            _AC(0x1000,UL)
//  0x1001+4*i   1B/input    RW        clicintie[i]
#define CLIC_INT_IE_OFFSET            _AC(0x1001,UL)
//  0x1002+4*i   1B/input    RW        clicintattr[i]
#define CLIC_INT_ATTR_OFFSET          _AC(0x1002,UL)
//  0x1003+4*i   1B/input    RW        clicintctl[i]
#define CLIC_INT_CTRL_OFFSET          _AC(0x1003,UL)
//
//  ...
//
#define CLIC_ADDR_BASE           SOC_CLIC_CTRL_ADDR

#define CLIC_NUM_INTERRUPTS (SOC_CLIC_NUM_INTERRUPTS + 19)

#define CLIC_CFG_NLBITS_MASK          _AC(0x1E,UL)
#define CLIC_CFG_NLBITS_LSB     (1u)

#define CLIC_INT_SFT         3
#define CLIC_INT_TMR         7
#define CLIC_INT_IMECCI      16
#define CLIC_INT_BWEI        17
#define CLIC_INT_PMOVI       18

#define MTIME_HANDLER   clic_mtip_handler
#define SSIP_HANDLER    clic_ssip_handler 
#define MSIP_HANDLER    clic_msip_handler 
#define UTIP_HANDLER    clic_utip_handler 
#define STIP_HANDLER    clic_stip_handler 
#define IRQ7_HANDLER    clic_irq7_handler 
#define UEIP_HANDLER    clic_ueip_handler 
#define SEIP_HANDLER    clic_seip_handler 
#define MEIP_HANDLER    clic_meip_handler 
#define IMECCI_HANDLER  clic_imecci_handler
#define BWEI_HANDLER    clic_bwei_handler
#define PMOVI_HANDLER   clic_pmovi_handler


#endif
