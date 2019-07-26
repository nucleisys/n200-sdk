TARGET := dhrystone

C_SRCS := dhry_stubs.c dhry_1.c dhry_2.c 
HEADERS := dhry.h

CFLAGS := -O3 -fno-inline -funroll-loops -Wno-implicit -mexplicit-relocs -save-temps  -fno-builtin-printf -fno-common -falign-functions=4 -falign-jumps=4 -falign-loops=4


BSP_BASE = ../../bsp
include $(BSP_BASE)/$(BOARD)/n200/env/common.mk

