#include <iostream>
#include <cstring>
#include <stdio.h>
#include <vector>
#include <memory>
#include <stdlib.h>
#include "verilated_vcd_c.h"
#include "Vsoc_tb.h"

//mid: 1 core 2 caravel 3 testio(outside) 4 uart(outside)
struct SOC {
    Vsoc_tb* logic;
    VerilatedContext* contextp;
    VerilatedVcdC* tfp;
    void reset();
    void cycle();
    void init();
    void master_write(uint32_t address, uint32_t wdata, int mid);
    void master_read(uint32_t address, int mid);
    void close();
};

void SOC::init(){
    contextp = new VerilatedContext;
    logic = new Vsoc_tb(contextp);
    tfp = new VerilatedVcdC;
    contextp->traceEverOn(true); 
    logic->trace(tfp, 0);
    tfp->open("soc_waves.vcd");
}

void SOC::cycle(){
    contextp->timeInc(HALF_CYCLE);
    logic->clk = 0;
    logic->testio_clk = 0;
    logic->eval();
    tfp->dump(contextp->time());
    contextp->timeInc(HALF_CYCLE);
    logic->clk = 1;
    logic->testio_clk = 1;
    logic->eval();
    tfp->dump(contextp->time());
} // 每个cycle后给外界表现的是上升沿之后的信号

void SOC::reset(){
    logic->clk = 1;
    logic->testio_clk = 1;
    logic->rst = 0;
    logic->eval();
    int i = 0;
    while (i < 10){
        cycle();
        i++;
    }
    //此时位于下降沿底部，且已经计算完成
    contextp->timeInc(HALF_CYCLE*0.5);
    logic->rst = 1;
    logic->eval();
    tfp->dump(contextp->time());
    contextp->timeInc(HALF_CYCLE*0.5);
    logic->clk = 0;
    logic->testio_clk = 0;
    logic->eval();
    tfp->dump(contextp->time());
    //复位完成后在上升沿顶部，且已经计算完成
    contextp->timeInc(HALF_CYCLE);
    logic->clk = 1;
    logic->testio_clk = 1;
    logic->eval();
    tfp->dump(contextp->time());
}

void SOC::master_write(uint32_t address, uint32_t wdata, int mid){
    switch(mid){
    case 1:
        logic->m1_cyc = 1;
        logic->m1_stb = 1;
        logic->m1_we = 1;
        logic->m1_wdata = wdata;
        logic->m1_sel = 15;
        logic->m1_addr = address;
        int time_limit = 0;
        while(time_limit < 1000 && !logic->m1_ack){
            cycle();
            time_limit++;
        }
        if(logic->m1_ack){
            printf("the process that core write data 0x%x to address 0x%x succeed.\n", wdata, address);
            logic->m1_cyc = 0;
            logic->m1_stb = 0;
            cycle();
        } else if(time_limit == 1000){
            printf("the process that core write data 0x%x to address 0x%x failed.\n", wdata, address);
            assert(0);
        }
    case 2:
        logic->m2_cyc = 1;
        logic->m2_stb = 1;
        logic->m2_we = 1;
        logic->m2_wdata = wdata;
        logic->m2_sel = 15;
        logic->m2_addr = address;
        int time_limit = 0;
        while(time_limit < 1000 && !logic->m2_ack){
            cycle();
            time_limit++;
        }
        if(logic->m2_ack){
            printf("the process that caravel write data 0x%x to address 0x%x succeed.\n", wdata, address);
            logic->m2_cyc = 0;
            logic->m2_stb = 0;
            cycle();
        } else if(time_limit == 1000){
            printf("the process that caravel write data 0x%x to address 0x%x failed.\n", wdata, address);
            assert(0);
        }
        break;
    case 3:
        logic->m3_cyc = 1;
        logic->m3_stb = 1;
        logic->m3_we = 1;
        logic->m3_wdata = wdata;
        logic->m3_sel = 15;
        logic->m3_addr = address;
        int time_limit = 0;
        while(time_limit < 1000 && !logic->m3_ack){
            cycle();
            time_limit++;
        }
        if(logic->m3_ack){
            printf("the process that testio write data 0x%x to address 0x%x succeed.\n", wdata, address);
            logic->m3_cyc = 0;
            logic->m3_stb = 0;
            cycle();
        } else if(time_limit == 1000){
            printf("the process that testio write data 0x%x to address 0x%x failed.\n", wdata, address);
            assert(0);
        }
        break;
    case 4:
        if(address != 0x2000300){
            printf("uart writing address wrong!\n");
            break;
        }
        logic->m4_cyc = 1;
        logic->m4_stb = 1;
        logic->m4_we = 1;
        logic->m4_wdata = wdata;
        logic->m4_sel = 15;
        logic->m4_addr = address;
        int time_limit = 0;
        while(time_limit < 1000 && !logic->m4_ack){
            cycle();
            time_limit++;
        }
        if(logic->m4_ack){
            printf("the process that uart send data 0x%x succeed.\n", wdata);
            logic->m4_cyc = 0;
            logic->m4_stb = 0;
            cycle();
        } else if(time_limit == 1000){
            printf("the process that uart send data 0x%x failed.\n", wdata);
            assert(0);
        }
        break;
    default:
        printf("wrong master id!\n");
        break;
    }
}


void SOC::master_write(uint32_t address, uint32_t wdata, int mid){
    switch(mid){
    case 1:
        logic->m1_cyc = 1;
        logic->m1_stb = 1;
        logic->m1_we = 0;
        logic->m1_addr = address;
        int time_limit = 0;
        while(time_limit < 1000 && !logic->m1_ack){
            cycle();
            time_limit++;
        }
        if(logic->m1_ack){
            printf("the process that core read data from address 0x%x succeed, the value is 0x%x.\n", address, logic->rdata);
            logic->m1_cyc = 0;
            logic->m1_stb = 0;
            cycle();
        } else if(time_limit == 1000){
            printf("the process that core read data from address 0x%x failed.\n", address);
            assert(0);
        }
    case 2:
        logic->m2_cyc = 1;
        logic->m2_stb = 1;
        logic->m2_we = 0;
        logic->m2_addr = address;
        int time_limit = 0;
        while(time_limit < 1000 && !logic->m2_ack){
            cycle();
            time_limit++;
        }
        if(logic->m2_ack){
            printf("the process that caravel read data from address 0x%x succeed, the value is 0x%x.\n", address, logic->rdata);
            logic->m2_cyc = 0;
            logic->m2_stb = 0;
            cycle();
        } else if(time_limit == 1000){
            printf("the process that caravel read data from address 0x%x failed.\n", address);
            assert(0);
        }
        break;
    case 3:
        logic->m3_cyc = 1;
        logic->m3_stb = 1;
        logic->m3_we = 0;
        logic->m3_addr = address;
        int time_limit = 0;
        while(time_limit < 1000 && !logic->m3_ack){
            cycle();
            time_limit++;
        }
        if(logic->m3_ack){
            printf("the process that testio read data from address 0x%x succeed, the value is 0x%x.\n", address, logic->rdata);
            logic->m3_cyc = 0;
            logic->m3_stb = 0;
            cycle();
        } else if(time_limit == 1000){
            printf("the process that testio read data from address 0x%x failed.\n", address);
            assert(0);
        }
        break;
    case 4:
        if(address != 0x200030c){
            printf("uart reading address wrong!\n");
            break;
        }
        logic->m4_cyc = 1;
        logic->m4_stb = 1;
        logic->m4_we = 0;
        logic->m4_addr = address;
        int time_limit = 0;
        while(time_limit < 1000 && !logic->m4_ack){
            cycle();
            time_limit++;
        }
        if(logic->m4_ack){
            printf("the process that uart receive data 0x%x succeed.\n", logic->rdata);
            logic->m4_cyc = 0;
            logic->m4_stb = 0;
            cycle();
        } else if(time_limit == 1000){
            printf("the process that uart receive data failed.\n");
            assert(0);
        }
        break;
    default:
        printf("wrong master id!\n");
        break;
    }
}

void SOC::close(){
    logic->final();
    delete logic;
    tfp->close();
    delete contextp;
}

int main(){
    SOC my_soc;
    soc.init();
    soc.reset();

    // soc.master_read();
    
    soc.close();
}