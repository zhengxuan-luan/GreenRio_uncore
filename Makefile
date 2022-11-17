SIM_SRC = $(DV_ROOT)/soc_sim.cpp
VERILOG_SRC := $(shell find $(SRC_ROOT) -type f -name '*.[v]')
VERILOG_SRC += perips_cfg.vh

VERILATOR := /work/stu/zxluan/verilator/bin/verilator
VERIFLAGS := -Mdir build --timescale 1ns/1ps --timescale-override 1ns/1ps --prof-cfuncs -CFLAGS -DVL_DEBUG\
		  --Wno-WIDTH --Wno-PINMISSING --Wno-VARHIDDEN --Wno-UNUSED --Wno-STMTDLY --Wno-ASSIGNDLY --Wno-EOFNEWLINE\
		  --Wno-IMPLICIT --cc --trace --exe --build
.PHONY: verify_soc sim

sim: $(VERILOG_SRC) $(SIM_SRC)
# @echo $(shell echo  $(VERILOG_SRC) | grep vh)
	$(VERILATOR) $(VERIFLAGS) $(DV_ROOT)/soc_tb.v $(VERILOG_SRC) $(SIM_SRC)

verify_soc: sim
	-build/Vsoc_tb
