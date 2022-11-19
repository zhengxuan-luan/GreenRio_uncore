#ifndef _FAKE_MEM_CPP_
#define _FAKE_MEM_CPP_
#include "./fake_mem.hpp"

uint8_t FakeMemory::get_byte(const uint16_t& paddr){
    return _mem[paddr];
}
void FakeMemory::set_byte(const uint16_t&  paddr, const uint8_t& data){
    _mem[paddr] = data;
}

uint8_t FakeMemory::pagetable_get_byte(const uint16_t&  paddr){
    return _pagetable[paddr];
}
void FakeMemory::pagetable_set_byte(const uint16_t&  paddr, const uint8_t& data){
    _pagetable[paddr] = data;
}

void FakeMemory::ar_vld(const bool& vld){
    _ar_vld = vld;
}
void FakeMemory::ar_req(const axi_ar_req& ar){
    _ar_req = ar;
}
void FakeMemory::aw_vld(const bool& vld){
    _aw_vld = vld;
}
void FakeMemory::aw_req(const axi_aw_req& aw){
    _aw_req = aw;
}
void FakeMemory::w_vld(const bool& vld){
    _w_vld = vld;
}
void FakeMemory::w_req(const axi_w_req& w){
    _w_req = w;
}
void FakeMemory::b_rdy(const bool & rdy){
    _b_rdy = rdy;
}
void FakeMemory::r_rdy(const bool & rdy){
    _r_rdy = rdy;
}
bool FakeMemory::r_vld() const{
    return _r_vld;
}
axi_r_resp FakeMemory::r_resp() const{
    return _r_resp;
}
bool FakeMemory::b_vld() const{
    return _b_vld;
}
axi_b_resp FakeMemory::b_resp() const{
    return _b_resp;
}
uint64_t FakeMemory::visit_time() const{
    return _visit_time;
}
void FakeMemory::eval(){
    // enque req
    if(_aw_vld){
        mem_req req;
        req.aw_vld = 1;
        req.aw = _aw_req;
        req.delay = MEM_BASE_DELAY + random() %  MEM_MAX_DELAY;
        _req_wait_q->push_back(req);
#ifdef LOG_ENABLE
        LOG << "t " << std::dec << TIME << ":axi mem aw req recieved.\t\t"
            << "@ 0x" << std::hex  << req.aw.awaddr << "\t";
        LOG << std::endl;
#endif // LOG_ENABLE
    }
    if(_ar_vld){
        mem_req req;
        req.ar_vld = 1;
        req.ar = _ar_req;
        req.delay = MEM_BASE_DELAY + random() %  MEM_MAX_DELAY;
        _req_wait_q->push_back(req);
#ifdef LOG_ENABLE
        LOG << "t " << std::dec << TIME << ":axi mem ar req recieved.\t\t"
            << "@ 0x" << std::hex  << req.ar.araddr << "\t";
        LOG << std::endl;
#endif // LOG_ENABLE
    }
    if(_w_vld){
        bool flag = 0;
        for(auto it = _req_wait_q->begin(); it != _req_wait_q->end(); it ++){
            if(it->aw_vld && (it->w_vld_num < 8)){
                flag = 1;
                it->w[it->w_vld_num] = _w_req;
                it->w_vld_num  ++;
            }
        }
        if(!flag){
            throw "w vld while no aw is arrived first";
        }
    }
    
    // count down delay
    for(auto it = _req_wait_q->begin(); it != _req_wait_q->end(); it ++){
        if(it->delay > 0){
            it->delay --;
        }
    }

    // move req from wait queue
    while(_req_wait_q->front().delay == 0 && (_req_wait_q->front().ar_vld || (_req_wait_q->front().aw_vld && _req_wait_q->front().w_vld_num == 8))){
        _req_q->push_back(_req_wait_q->front());
        _req_wait_q->pop_front();
    }

    // handle req
    while(_req_q->size() > 0){
        if((_req_q->front().aw_vld && _req_q->front().w_vld_num == 8) || (_req_q->front().ar_vld)){
            mem_req req = _req_q->front();
            _req_q->pop_front();
            mem_resp resp;

            if(req.ar_vld){
                for(int i = 0; i < BURST_NUM; i ++){
                    uint64_t data = 0;
                    for(int j = 0; j < BURST_SIZE/8; j ++){
                        if(DATA_PADDR_BASE <= req.ar.araddr && req.ar.araddr < DATA_PADDR_UPPER){
                            data += _mem[uint16_t(req.ar.araddr + i * 8 + j)] << (j * 8);
                        }
                        else if (PAGETABLE_PADDR_BASE <= req.ar.araddr && req.ar.araddr < PAGETABLE_PADDR_UPPER){
                            data += _pagetable[uint16_t(req.ar.araddr + i * 8 + j)] << (j * 8);
                        }
                        else{
                            std::string msg = "invalid read access to memory";
                            throw msg;
                        }
                    }
                    resp.r[i].rid = req.ar.arid;
                    if(i == BURST_NUM - 1){
                        resp.r[BURST_NUM - 1].rlast = 1;
                    }
                    resp.r[i].rdata= data;
                    resp.r_vld_num ++;
#ifdef LOG_ENABLE
                    LOG << "t " << std::dec << TIME << ":axi mem r resp recieved.\t\t";
                    LOG << "data:";
                    for(uint32_t i = 0; i < BURST_NUM; i ++)
                        LOG << std::hex  << resp.r[BURST_NUM - i - 1].rdata << " ";
                    LOG << std::endl;
#endif // LOG_ENABLE
                }
            }
            else{
                for(int i = 0; i < BURST_NUM; i ++){
                    uint64_t data = 0;
                    for(int j = 0; j < BURST_SIZE/8; j ++){
                        if(DATA_PADDR_BASE <= req.ar.araddr && req.ar.araddr < DATA_PADDR_UPPER){
                            _mem[req.aw.awaddr + i * 8 + j] = uint8_t(req.w[i].wdata >> (j * 8));
                        }
                        else{
                            std::string msg = "invalid write access to memory";
                            throw msg;
                        }
                    }
                }
#ifdef LOG_ENABLE
                    LOG << "t " << std::dec << TIME << ":axi mem w wb.\t\t";
                    LOG << "@ :" << std::hex << req.aw.awaddr;
                    LOG << "data:";
                    for(uint32_t i = 0; i < BURST_NUM; i ++)
                        LOG << std::hex << resp.r[BURST_NUM - i - 1].rdata << " ";
                    LOG << std::endl;
#endif // LOG_ENABLE
                resp.b_vld = 1;
                resp.b.bid = req.aw.awid;
            }
            _resp_q->push_back(resp);
        }
    }

    // sent resp
    if(_resp_q->size() > 0){
        _b_vld = 0;
        _r_vld = 0;
        if(_resp_q->front().b_vld){
            _b_vld = 1;
            _b_resp = _resp_q->front().b;
            if(_b_rdy)
                _resp_q->pop_front();
        }
        if((_resp_q->front().r_vld_num == BURST_NUM) || _r_resp_on_flight){
            _r_vld = 1;
            _r_resp = _resp_q->front().r[BURST_NUM - _resp_q->front().r_vld_num];
            if(_r_rdy)
                _resp_q->front().r_vld_num --;
            if(_resp_q->front().r_vld_num == 0){
                _r_resp_on_flight = 0;
                _resp_q->pop_front();
            }
            else{
                _r_resp_on_flight = 1;
            }
        }
    }
}
#endif // _FAKE_MEM_CPP_