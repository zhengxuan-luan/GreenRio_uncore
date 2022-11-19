
VERIFLAGS := -Mdir build --timescale 1ns/1ps --timescale-override 1ns/1ps --prof-cfuncs -CFLAGS -DVL_DEBUG\
		  --Wno-WIDTH --Wno-PINMISSING --Wno-VARHIDDEN --Wno-UNUSED --Wno-STMTDLY --Wno-ASSIGNDLY --Wno-EOFNEWLINE\
		  --Wno-IMPLICIT --cc --trace --exe --build +define+VERIFY_SOC

.PHONY: verify_soc

verify_soc: $(VERILOG_SRC) $(SIM_SRC)
	verilator $(VERIFLAGS) $(DV_ROOT)/soc_ut/soc_tb.v $(shell find $(SRC_ROOT)/perips -type f -name '*.[v]' ! -name sky130_fd_sc_hd.v) perips_cfg.vh $(DV_ROOT)/soc_ut/soc_sim.cpp
	-build/Vsoc_tb

soc_waves:
	gtkwave soc_waves.vcd
