
# == Directories
#SRC_DIR   := src
SRC_NEW_DIR := rtl
BUILD_DIR := build
TEST_DIR  := tests
UNIT_TEST_DIR := unit_tests
SIM_DIR   := dv
# ==

# == Test files
#VERILOG_SRC := $(shell find $(SRC_DIR) -type f -name '*.v')
#VERILOG_SRC += $(shell find $(SRC_DIR) -type f -name '*.vh')
VERILOG_SRC := $(shell find $(SRC_NEW_DIR)/include/ -type f -name '*.v')
VERILOG_SRC := $(shell find $(SRC_NEW_DIR)/lsuv1/ -type f -name '*.v')
VERILOG_SRC += $(shell find $(SRC_NEW_DIR)/rvh_mmu/ -type f -name '*.v')
VERILOG_SRC += $(shell find $(SRC_NEW_DIR)/rvh_l1d/ -type f -name '*.sv')
VERILOG_SRC += $(shell find $(SRC_NEW_DIR)/rvh_l1d/ -type f -name '*.v')
VERILOG_SRC += $(shell find $(SRC_NEW_DIR)/utils/ -type f -name '*.v')
VERILOG_SRC += $(shell find $(SRC_NEW_DIR)/perips/ -type f -name '*.v')
VERILOG_SRC += $(SRC_NEW_DIR)/l1d_top.sv
# TEST_SRC    := $(shell find $(TEST_DIR) -type f -name '*.[Sc]')

TEST_BINS   := $(patsubst $(TEST_DIR)/%.c, $(TEST_DIR)/build/%, $(patsubst $(TEST_DIR)/%.S, $(TEST_DIR)/build/%, $(TEST_SRC)))
# ==

# == Simulator files
SIM_SRC := $(shell find $(SIM_DIR)/l1d -type f -name '*.cpp')
SIM_SRC += $(shell find $(SIM_DIR)/l1d -type f -name '*.hpp')
# ==

# == Runing goals
RUNTESTS  := $(addprefix RUNTEST.,$(TEST_BINS))
# ==

# == Verilator config
VERILATOR := /work/stu/zxluan/verilator/bin/verilator

VERIFLAGS := $(addprefix -I,$(SRC_NEW_DIR)/lsuv1)
VERIFLAGS += $(addprefix -I,$(SRC_NEW_DIR)/units/commoncell/oldest2_abitter)
VERIFLAGS += $(addprefix -I,$(SRC_NEW_DIR)/units/commoncell/counter)
VERIFLAGS += $(addprefix -I,$(shell find $(SRC_NEW_DIR)/rvh_mmu -type d)) 
VERIFLAGS += $(addprefix -I,$(shell find $(SRC_NEW_DIR)/rvh_l1d -type d)) 
VERIFLAGS += $(addprefix -I,$(shell find $(SRC_NEW_DIR)/include -type d)) 
VERIFLAGS += $(addprefix -I,$(shell find $(SRC_NEW_DIR)/utils -type d)) 
VERIFLAGS += $(addprefix -I,$(shell find $(SRC_NEW_DIR) -type d)) 
VERIFLAGS += -Wall -Mdir $(BUILD_DIR)

# ==

# .SILENT:
.SECONDARY:
.SECONDEXPANSION:
.PHONY: test sim build-tests RUNTEST.$(TEST_DIR)/build/% $(TEST_DIR)/build clean

test: $(RUNTESTS)

clean:
	-rm -rf build/*
	-rm -rf tests/build/
	-rm *.vcdrvh_l1d_ewrq.sv
	-rm *.log

sim: $(BUILD_DIR)/Vhehecore

default: l1d

l1d:$(BUILD_DIR)/l1d
$(BUILD_DIR)/l1d: $(VERILOG_SRC) $(SIM_SRC) | $$(dir $$@)
	echo $(VERILOG_SRC)
	@echo Building $@
	$(VERILATOR) $(VERIFLAGS) --trace --cc --exe --build --top-module l1d_top \
	-Wno-UNUSED -Wno-UNOPTFLAT -Wno-PINMISSING -Wno-EOFNEWLINE -Wno-PINCONNECTEMPTY -Wno-VARHIDDEN -Wno-WIDTH \
	+define+USE_VERILATOR+LSU_DEBUG \
	-LDFLAGS -lelf $(SRC_NEW_DIR)/l1d_top.sv $(SIM_DIR)/l1d/l1d.cpp 
	./build/Vl1d_top