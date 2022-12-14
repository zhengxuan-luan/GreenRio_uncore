
VERIFLAGS := -Mdir build --timescale 1ns/1ps --timescale-override 1ns/1ps --prof-cfuncs -CFLAGS -DVL_DEBUG\
		  --Wno-WIDTH --Wno-PINMISSING --Wno-VARHIDDEN --Wno-UNUSED --Wno-STMTDLY --Wno-ASSIGNDLY --Wno-EOFNEWLINE\
		  --Wno-IMPLICIT --Wno-LITENDIAN --Wno-SELRANGE --Wno-UNOPTFLAT --Wno-CASEINCOMPLETE --cc --trace --exe --build +define+VERIFY_SOC

SOC_SRC := $(shell find $(SRC_ROOT)/perips -type f -name '*.[v]' ! -name sky130_fd_sc_hd.v) perips_cfg.vh

CACHE_SRC := $(shell find $(SRC_ROOT)/cache -type f -name '*.[v]')
CACHE_SRC += $(shell find $(SRC_ROOT)/cache_ebi -type f -name '*.[v]')

.PHONY: verify_soc verify_cache

verify_soc:
	verilator $(VERIFLAGS) $(DV_ROOT)/soc_ut/soc_tb.v $(SOC_SRC) $(DV_ROOT)/soc_ut/soc_sim.cpp
	-build/Vsoc_tb

verify_cache: 
	verilator $(VERIFLAGS) -top-module cache_utop +define+RUBY --Wno-LATCH --Wno-COMBDLY $(CACHE_SRC)  $(DV_ROOT)/cache_ut/cache_utop.v $(DV_ROOT)/cache_ut/cache_sim.cpp
	-build/Vcache_utop

soc_waves:
	gtkwave soc_waves.vcd

cache_waves:
	gtkwave cache_waves.vcd