# See LICENSE for license details.

ifndef _NUCLEI_MK_COMMON
_NUCLEI_MK_COMMON := # defined

.PHONY: all
all: $(TARGET)

FLASHXIP    := flashxip
FLASH       := flash
ILM        := ilm
DOWNLOAD    := flash

SOC_DRIVER_DIR = $(BSP_BASE)/$(BOARD)/soc/drivers

N22_DRIVER_DIR = $(BSP_BASE)/$(BOARD)/n22/drivers
N22_ENV_DIR = $(BSP_BASE)/$(BOARD)/n22/env
N22_STUB_DIR = $(BSP_BASE)/$(BOARD)/n22/stubs

   # The start-up assembly program
ASM_SRCS += $(N22_ENV_DIR)/start.S
   # The system initilization program
C_SRCS += $(N22_ENV_DIR)/init.c
   # The interrupt/exception/nmi entry program
ASM_SRCS += $(N22_ENV_DIR)/entry.S
   # The interrupt/exception/nmi handler program
C_SRCS += $(N22_ENV_DIR)/handlers.c
   # The processor core common functions
C_SRCS += $(N22_DRIVER_DIR)/n22_func.c
   # The clcic driver
C_SRCS += $(N22_DRIVER_DIR)/clic_driver.c
   # The newlib stubs functions
C_SRCS += $(N22_STUB_DIR)/_exit.c
C_SRCS += $(N22_STUB_DIR)/write_hex.c
C_SRCS += $(N22_STUB_DIR)/write.c
C_SRCS += $(N22_STUB_DIR)/close.c
C_SRCS += $(N22_STUB_DIR)/fstat.c
C_SRCS += $(N22_STUB_DIR)/isatty.c
C_SRCS += $(N22_STUB_DIR)/lseek.c
C_SRCS += $(N22_STUB_DIR)/read.c
C_SRCS += $(N22_STUB_DIR)/sbrk.c

   # The SoC common functions
C_SRCS += $(SOC_DRIVER_DIR)/soc_func.c

ifeq ($(DOWNLOAD),${FLASH}) 
LINKER_SCRIPT := $(N22_ENV_DIR)/link_flash.lds
endif

ifeq ($(DOWNLOAD),${ILM}) 
LINKER_SCRIPT := $(N22_ENV_DIR)/link_ilm.lds
endif

ifeq ($(DOWNLOAD),${FLASHXIP}) 
LINKER_SCRIPT := $(N22_ENV_DIR)/link_flashxip.lds
endif

INCLUDES += -I$(BSP_BASE)/$(BOARD)

LDFLAGS += -T $(LINKER_SCRIPT)  -nostartfiles -Wl,--gc-sections  -Wl,--check-sections
LDFLAGS += --specs=nano.specs --specs=nosys.specs

ifeq ($(NANO_PFLOAT),1) 
LDFLAGS += -u _printf_float 
endif


LDFLAGS += -L$(N22_ENV_DIR)

ASM_OBJS := $(ASM_SRCS:.S=.o)
C_OBJS := $(C_SRCS:.c=.o)
DUMP_OBJS := $(C_SRCS:.c=.dump)
VERILOG_OBJS := $(C_SRCS:.c=.verilog)

LINK_OBJS += $(ASM_OBJS) $(C_OBJS)
LINK_DEPS += $(LINKER_SCRIPT)

CLEAN_OBJS += $(TARGET) $(LINK_OBJS) $(DUMP_OBJS) $(VERILOG_OBJS)

CFLAGS += -g
CFLAGS += -march=$(RISCV_ARCH)
CFLAGS += -mabi=$(RISCV_ABI)
CFLAGS += -ffunction-sections -fdata-sections -fno-common


$(TARGET): $(LINK_OBJS) $(LINK_DEPS)
	$(CC) $(CFLAGS) $(INCLUDES) $(LINK_OBJS) -o $@ $(LDFLAGS)
	$(SIZE) $@

$(ASM_OBJS): %.o: %.S $(HEADERS)
	$(CC) $(CFLAGS) $(INCLUDES) -c -o $@ $<

$(C_OBJS): %.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) $(INCLUDES) -include sys/cdefs.h -c -o $@ $<

.PHONY: clean
clean:
	rm -f $(CLEAN_OBJS)

endif # _NUCLEI_MK_COMMON
