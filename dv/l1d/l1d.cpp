/////////////////////////
// Author: Peichen Guo //
//    RIOS Lab work    //
//      HeHe Core      //
/////////////////////////
#include "Vl1d_top.h"

#include <iostream>
#include <string> 
#include <verilated.h>
#include "verilated_vcd_c.h"
#include <queue> 
#include <stdio.h>

#include "include.hpp"

// const uint64_t MAX_TIME = 30000000;
const uint64_t MAX_TIME = 4000000000;
const uint64_t RANDOM_TEST_NUM = 10000000;
const uint32_t RST_TIME = 100;


uint64_t main_time = 0;
Vl1d_top* l1d;

double sc_time_stamp(){
    return main_time;
}

void reset(){

   
    return;
}
int main(int argc, char ** argv, char** env) {
    Verilated::commandArgs(argc, argv);
    Verilated::traceEverOn(true);
    VerilatedVcdC* tfp = new VerilatedVcdC();
    FakeMemory* real_mem = nullptr;
    FakeMemory* perfect_mem = nullptr;
    try{
        real_mem = new FakeMemory("./dv/l1d/pagetable");
        perfect_mem = new FakeMemory("./dv/l1d/pagetable");
    }
    catch (const char* msg){
        printf(msg);
        delete tfp;
        if(real_mem != nullptr)
            delete real_mem;
        if(perfect_mem != nullptr)
            delete perfect_mem;
    }
    

    l1d = new Vl1d_top("l1d");
    l1d->trace(tfp, 0);
    tfp->open("l1d.vcd");

    l1d->clk = 0;
    l1d->rst = 1;
    l1d->flush = 0;

    reset();

    
    try {
        // test code 
        
    }
    catch(const char* msg){
       
        delete tfp;
        if(real_mem != nullptr)
            delete real_mem;
        if(perfect_mem != nullptr)
            delete perfect_mem;
    }
    tfp->dump(main_time);
    tfp->close();

    delete tfp;
    if(real_mem != nullptr)
        delete real_mem;
    if(perfect_mem != nullptr)
        delete perfect_mem;
    exit(0);
}