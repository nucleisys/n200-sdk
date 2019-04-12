#############################################################
# Configuration
#############################################################

# Allows users to create Makefile.local or ../Makefile.project with
# configuration variables, so they don't have to be set on the command-line
# every time.
extra_configs := $(wildcard Makefile.local ../Makefile.project)
ifneq ($(extra_configs),)
$(info Obtaining additional make variables from $(extra_configs))
include $(extra_configs)
endif

SIMULATION     := 0

FLASHXIP    := flashxip
FLASH       := flash
ILM         := ilm
DOWNLOAD    := ${FLASH}

OCDCFG := hbird
HBIRD  := hbird
OLMX   := olmx

N201        := n201
N203        := n203
N205        := n205
N205F       := n205f
N205FD      := n205fd

CORE        := n205

CORE_NAME = $(shell echo $(CORE) | tr a-z A-Z)
core_name = $(shell echo $(CORE) | tr A-Z a-z)

PFLOAT     := 1


ifeq ($(core_name),${N201}) 
RISCV_ARCH := rv32iac
RISCV_ABI  := ilp32
endif

ifeq ($(core_name),${N203}) 
RISCV_ARCH := rv32imac
RISCV_ABI  := ilp32
endif

ifeq ($(core_name),${N205}) 
RISCV_ARCH := rv32imac
RISCV_ABI  := ilp32
endif

ifeq ($(core_name),${N207F}) 
RISCV_ARCH := rv32imafc
RISCV_ABI  := ilp32f
endif

ifeq ($(core_name),${N207FD}) 
RISCV_ARCH := rv32imafdc
RISCV_ABI  := ilp32f
endif





PFLOAT     := 1
YOUR_PRINTF     := 0




# Default target
BOARD ?= nuclei-n200
PROGRAM ?= dhrystone
GDB_PORT ?= 3333

# Variables the user probably shouldn't override.
builddir := work/build
installdir := work/install
toolchain_srcdir := riscv-gnu-toolchain
openocd_srcdir := openocd

#############################################################
# BSP Loading
#############################################################

# Finds the directory in which this BSP is located, ensuring that there is
# exactly one.
board_dir := $(wildcard bsp/$(BOARD))
ifeq ($(words $(board_dir)),0)
$(error Unable to find BSP for $(BOARD), expected to find either "bsp/$(BOARD)" or "bsp-addons/$(BOARD)")
endif
ifneq ($(words $(board_dir)),1)
$(error Found multiple BSPs for $(BOARD): "$(board_dir)")
endif


ifeq ($(RISCV_ARCH),)
$(error $(board_dir)/board.mk must set RISCV_ARCH, the RISC-V ISA string to target)
endif

ifeq ($(RISCV_ABI),)
$(error $(board_dir)/board.mk must set RISCV_ABI, the ABI to target)
endif


#############################################################
# Prints help message
#############################################################
.PHONY: help
help:
	@echo "  Nuclei N200 RISC-V Embedded Processor Software Development Kit "
	@echo "  Makefile targets:"
	@echo ""
	@echo " tools [BOARD = $(BOARD)]:"
	@echo "    Install compilation & debugging tools to target your desired board."
	@echo ""
	@echo " uninstall:"
	@echo "    Uninstall the compilation & debugging tools."
	@echo ""
	@echo " software [PROGRAM=$(PROGRAM) BOARD=$(BOARD)]:"
	@echo "    Build a software program to load with the"
	@echo "    debugger."
	@echo ""
	@echo " upload [PROGRAM=$(PROGRAM) BOARD=$(BOARD)]:"
	@echo "    Launch OpenOCD to flash your program to the"
	@echo "    on-board Flash."
	@echo ""
	@echo " run_openocd [BOARD=$(BOARD)]:"
	@echo " run_gdb     [PROGRAM=$(PROGRAM) BOARD=$(BOARD)]:"
	@echo "     Launch OpenOCD or GDB seperately. Allows Ctrl-C to halt running"
	@echo "     programs."
	@echo ""
	@echo " dasm [PROGRAM=$(BOARD)]:"
	@echo "     Generates the dissassembly output of 'objdump -D' to stdout."
	@echo ""

.PHONY: clean
clean:

#############################################################
# This section is for tool installation
#############################################################
.PHONY: tools
tools: riscv-gnu-toolchain openocd

# Pointers to various important tools in the toolchain.
toolchain_builddir := $(builddir)/riscv-gnu-toolchain/riscv32-unknown-elf
toolchain_prefix := $(toolchain_builddir)/prefix

RISCV_PATH ?= $(toolchain_prefix)

RISCV_GCC     := $(abspath $(RISCV_PATH)/bin/riscv64-unknown-elf-gcc)
RISCV_GXX     := $(abspath $(RISCV_PATH)/bin/riscv64-unknown-elf-g++)
RISCV_OBJDUMP := $(abspath $(RISCV_PATH)/bin/riscv64-unknown-elf-objdump)
RISCV_OBJCOPY := $(abspath $(RISCV_PATH)/bin/riscv64-unknown-elf-objcopy)
RISCV_GDB     := $(abspath $(RISCV_PATH)/bin/riscv64-unknown-elf-gdb)
RISCV_AR      := $(abspath $(RISCV_PATH)/bin/riscv64-unknown-elf-ar)
RISCV_SIZE    := $(abspath $(RISCV_PATH)/bin/riscv64-unknown-elf-size)

PATH := $(abspath $(RISCV_PATH)/bin):$(PATH)

$(RISCV_GCC) $(RISCV_GXX) $(RISCV_OBJDUMP) $(RISCV_GDB) $(RISCV_AR): $(toolchain_builddir)/install.stamp
	touch -c $@

# Builds riscv-gnu-toolchain, which contains GCC and all the supporting
# software for C code.
.PHONY: riscv-gnu-toolchain
riscv-gnu-toolchain: $(RISCV_GCC) $(RISCV_GXX) $(RISCV_OBJDUMP) $(RISCV_GDB) $(RISCV_AR)

$(builddir)/riscv-gnu-toolchain/%/install.stamp: $(builddir)/riscv-gnu-toolchain/%/build.stamp
	$(MAKE) -C $(dir $@) install
	date > $@

$(builddir)/riscv-gnu-toolchain/%/build.stamp: $(builddir)/riscv-gnu-toolchain/%/configure.stamp
	$(MAKE) -C $(dir $@)
	date > $@

$(builddir)/riscv-gnu-toolchain/%-elf/configure.stamp:
	$(eval $@_TUPLE := $(patsubst $(builddir)/riscv-gnu-toolchain/%-elf/configure.stamp,%,$@))
	rm -rf $(dir $@)
	mkdir -p $(dir $@)
	cd $(dir $@); $(abspath $(toolchain_srcdir)/configure) \
		--prefix=$(abspath $(dir $@)/prefix) \
		--disable-linux \
		--enable-multilib \
		--with-cmodel=medany \
		--with-libgcc-cmodel
	date > $@

.PHONY: 
clean: 

# Builds and installs OpenOCD, which translates GDB into JTAG for debugging and
# initializing the target.
openocd_builddir := $(builddir)/openocd
openocd_prefix := $(openocd_builddir)/prefix

RISCV_OPENOCD_PATH ?= $(openocd_prefix)
RISCV_OPENOCD ?= $(RISCV_OPENOCD_PATH)/bin/openocd

.PHONY: openocd
openocd: $(RISCV_OPENOCD)

$(RISCV_OPENOCD): $(openocd_builddir)/install.stamp
	touch -c $@

$(openocd_builddir)/install.stamp: $(openocd_builddir)/build.stamp
	$(MAKE) -C $(dir $@) install
	date > $@

$(openocd_builddir)/build.stamp: $(openocd_builddir)/configure.stamp
	$(MAKE) -C $(dir $@)
	date > $@

$(openocd_builddir)/configure.stamp:
	rm -rf $(dir $@)
	mkdir -p $(dir $@)
	cd $(abspath $(openocd_srcdir)); autoreconf -i
	cd $(dir $@); $(abspath $(openocd_srcdir)/configure) \
		--prefix=$(abspath $(dir $@)/prefix) \
		--disable-werror
	date > $@

.PHONY: 
clean: 

#############################################################
# This Section is for Software Compilation
#############################################################
PROGRAM_DIR = software/$(PROGRAM)
PROGRAM_ELF = software/$(PROGRAM)/$(PROGRAM)

.PHONY: software_clean
software_clean:
	$(MAKE) -C $(PROGRAM_DIR) BSP_BASE=$(abspath bsp) BOARD=$(BOARD)  DOWNLOAD=$(DOWNLOAD) clean

.PHONY: software
software: software_clean
	$(MAKE) -C $(PROGRAM_DIR) SIZE=$(RISCV_SIZE) CC=$(RISCV_GCC) RISCV_ARCH=$(RISCV_ARCH) YOUR_PRINTF=$(YOUR_PRINTF)  PFLOAT=$(PFLOAT)  DOWNLOAD=$(DOWNLOAD)  RISCV_ABI=$(RISCV_ABI) AR=$(RISCV_AR) BSP_BASE=$(abspath bsp) BOARD=$(BOARD) SIMULATION=$(SIMULATION)

dasm: software 
	$(RISCV_OBJDUMP) -D $(PROGRAM_ELF) >& $(PROGRAM_ELF).dump
	$(RISCV_OBJCOPY) $(PROGRAM_ELF) -O verilog $(PROGRAM_ELF).verilog
	sed -i 's/@800/@000/g' $(PROGRAM_ELF).verilog



#############################################################
# This Section is for uploading a program to SPI Flash
#############################################################
OPENOCD_UPLOAD = bsp/${BOARD}/tools/openocd_upload.sh
ifeq ($(OCDCFG),${OLMX})
ifeq ($(DOWNLOAD),${ILM})
OPENOCDCFG ?= bsp/$(BOARD)/n200/env/openocd_olmx_ilm.cfg
else
OPENOCDCFG ?= bsp/$(BOARD)/n200/env/openocd_olmx.cfg
endif
endif

ifeq ($(OCDCFG),${HBIRD})
ifeq ($(DOWNLOAD),${ILM})
OPENOCDCFG ?= bsp/$(BOARD)/n200/env/openocd_hbird_ilm.cfg
else
OPENOCDCFG ?= bsp/$(BOARD)/n200/env/openocd_hbird.cfg
endif
endif

OPENOCDARGS += -f $(OPENOCDCFG)

GDB_UPLOAD_ARGS ?= --batch

GDB_UPLOAD_CMDS += -ex "set remotetimeout 240"
GDB_UPLOAD_CMDS += -ex "target extended-remote localhost:$(GDB_PORT)"
GDB_UPLOAD_CMDS += -ex "monitor reset halt"
GDB_UPLOAD_CMDS += -ex "monitor flash protect 0 0 last off"
GDB_UPLOAD_CMDS += -ex "load"
GDB_UPLOAD_CMDS += -ex "monitor resume"
GDB_UPLOAD_CMDS += -ex "monitor shutdown"
GDB_UPLOAD_CMDS += -ex "quit"

openocd_upload:
	$(OPENOCD_UPLOAD) $(PROGRAM_ELF) $(OPENOCDCFG)

upload:
	$(RISCV_OPENOCD) $(OPENOCDARGS) & \
	$(RISCV_GDB) $(PROGRAM_DIR)/$(PROGRAM) $(GDB_UPLOAD_ARGS) $(GDB_UPLOAD_CMDS) && \
	echo "Successfully uploaded '$(PROGRAM)' to $(BOARD)."

#############################################################
# This Section is for launching the debugger
#############################################################

run_openocd:
	$(RISCV_OPENOCD) $(OPENOCDARGS)

GDBCMDS += -ex "set remotetimeout 240"
GDBCMDS += -ex "target extended-remote localhost:$(GDB_PORT)"

run_gdb:
	$(RISCV_GDB) $(PROGRAM_DIR)/$(PROGRAM) $(GDBARGS) $(GDBCMDS)
