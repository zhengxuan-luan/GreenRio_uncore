SIM_LOG = $(PROJ_ROOT)/$(SIM_ROOT)/logs
TIMESTAMP = $(shell date +%Y%m%d_%H%M)
dump_on = 1
PROJ_ROOT = $(shell pwd)
SIM_ROOT = sim/lsu_l1d
INCLUDE = $(PROJ_ROOT)/$(SIM_ROOT)/include
# default:
# 	vcs +vcs+lic+wait -sverilog -kdb +vc +error+10 -CC "-I$(INCLUDE)" -F "$(FLIST)"  +lint=TFIPC-L -full64 -timescale=1ns/1ps -debug_access -l bld.log

VCS_MACRO = 
VCS_PARAM =
CC_FILE =
BLD = 
VCS_MODEL_OPTS = 
CC_INC = $(INCLUDE)
CC_FLAGS = 
LDS_FILE = 
FILE_LIST = $(PROJ_ROOT)/flist/lsu_l1d_test.f
VFILE_LIST = $(PROJ_ROOT)/flist/lsu_vl1d_test.f
L1D_RUBY_FLIST = $(PROJ_ROOT)/flist/l1d_ruby.f
TOP_MODULE = l1d_test_top

bld:
	echo $(PROJ_ROOT)
	# -rm ./simv.daidir/.vcs.timestamp
	echo vcs +vcs+lic+wait -sverilog -kdb $(VCS_MACRO) $(VCS_PARAM) \
		+vc+list +vpi $(CC_FILE) \
		+error+3 \
		+define+VCS+LOG_LV0 \
		+define+PERFECT_MEM_FILE="\\\"$(PROJ_ROOT)/$(SIM_ROOT)/mem\\\"" \
		+define+PAGE_TABLE_FILE="\\\"$(PROJ_ROOT)/$(SIM_ROOT)/pagetable\\\"" \
		+define+ERROR_LOG_DIR="\\\"$(SIM_LOG)/error.log\\\"" \
		+define+RANDOM_TESTCASE_NUM="70000" \
		+define+MAX_TIME="700000000" \
		+define+SIM_DIR="\\\"$(PROJ_ROOT)/$(SIM_ROOT)\\\"" \
		+vcs+loopreport \
		-l $(SIM_LOG)/bld.log \
		-CFLAGS "$(CC_INC) $(CC_FLAGS)" \
		+lint=TFIPC-L \
		-full64 \
		-F "$(FILE_LIST)" \
		-top $(TOP_MODULE) -timescale=1ns/1ps \
		-v2k_generate -debug_access+all \
		> $(SIM_LOG)/rerun.sh
	bash $(SIM_LOG)/rerun.sh
	
vl1d_bld:
	echo $(PROJ_ROOT)
	# -rm ./simv.daidir/.vcs.timestamp
	echo vcs +vcs+lic+wait -sverilog -kdb $(VCS_MACRO) $(VCS_PARAM) \
		+vc+list +vpi $(CC_FILE) \
		+error+3 \
		+define+VCS+LOG_LV0 \
		+define+PERFECT_MEM_FILE="\\\"$(PROJ_ROOT)/$(SIM_ROOT)/mem\\\"" \
		+define+PAGE_TABLE_FILE="\\\"$(PROJ_ROOT)/$(SIM_ROOT)/pagetable\\\"" \
		+define+ERROR_LOG_DIR="\\\"$(SIM_LOG)/vl1d_error.log\\\"" \
		+define+RANDOM_TESTCASE_NUM="70000" \
		+define+MAX_TIME="700000000" \
		+define+SIM_DIR="\\\"$(PROJ_ROOT)/$(SIM_ROOT)\\\"" \
		+vcs+loopreport \
		-l $(SIM_LOG)/vl1d_bld.log \
		-CFLAGS "$(CC_INC) $(CC_FLAGS)" \
		+lint=TFIPC-L \
		-full64 \
		-F "$(VFILE_LIST)" \
		-top $(TOP_MODULE) -timescale=1ns/1ps \
		-v2k_generate -debug_access+all \
		> $(SIM_LOG)/vl1d_rerun.sh
	bash $(SIM_LOG)/vl1d_rerun.sh

#	vcs +vcs+lic+wait -sverilog -kdb $(VCS_MACRO) $(VCS_PARAM) \
		+vc+list +vpi $(CC_FILE) ${BLD}/rrvtb.so ${VCS_MODEL_OPTS} \
		-load ${BLD}/rrvtb.so:tb_init_cb \
		-CFLAGS "$(CC_INC) $(CC_FLAGS)" \
		-LDFLAGS "-lgmp -T$(LDS_FILE) -g" \
		+lint=TFIPC-L -full64 -F ${FILE_LIST} \
		-top $(TOP_MODULE) -timescale=1ns/1ps \
		-v2k_generate -debug_access+all

run:
	-mkdir $(SIM_LOG)
	time ./simv +dumpon=$(dump_on) +SIM_LOG=$(SIM_LOG)  2>&1 | tee $(SIM_LOG)/run_$(TIMESTAMP).log

test:memory bld run
vl1d_test:memory vl1d_bld run

wave:
	verdi -sv -ssf $(PROJ_ROOT)/$(SIM_ROOT)/waves/$(TOP_MODULE).fsdb

memory:
	python3 $(PROJ_ROOT)/$(SIM_ROOT)/generate_mem.py

clean:
	-rm $(SIM_LOG)/*.log
	-rm simv 
	-rm ucli.key 
	-rm waves.*
	-rm -rf csrc 
	-rm -rf simv.daidir 
	-rm -rf Verdi-SXLog
	-rm novas* .nfs* 

l1d_ruby:
	vcs +vcs+lic+wait -sverilog -kdb $(VCS_MACRO) $(VCS_PARAM) \
		+vc+list +vpi $(CC_FILE) \
		+error+10 \
		+define+VCS \
		+define+RUBY \
		+vcs+loopreport \
		-l $(SIM_LOG)/bld.log \
		-CFLAGS "$(CC_INC) $(CC_FLAGS)" \
		+lint=TFIPC-L \
		-full64 \
		-F "$(L1D_RUBY_FLIST)" \
		-top rvh_l1d -timescale=1ns/1ps \
		-v2k_generate -debug_access+all 