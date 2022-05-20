/* Implementation (skeleton)  for the dataflow statistics calculator */
#define INSIDE_DFLOW_CALC_C
#include "dflow_calc.h"
#include <vector>
#include <iostream>
#include <algorithm>

using namespace std;

class CmndInfo {
public:
    unsigned int inst_num;
    InstInfo info{};
    vector<CmndInfo *> dependencies;
    vector<CmndInfo *> dependee;

    CmndInfo(bool isBlock_, int dstIdx_, int src1Idx_, int src2Idx_, int src3Idx_,
             int opCode_, int InstID_, int inst_num_) {
        dependencies.push_back(nullptr);
        inst_num=inst_num_;
        info.isBlock = isBlock_;
        info.opcode = opCode_;
        info.src2Idx = src2Idx_;
        info.src1Idx = src1Idx_;
        info.src3Idx = src3Idx_;
        info.dstIdx = dstIdx_;
        info.InstID = InstID_;
    };
};

class Commands {
public:
    vector<CmndInfo *> insts;
    const unsigned int* latencies{};
    Commands() = default;
    unsigned int num_of_insts{};
    int numOfInstsInt;
    Commands &Init(const unsigned int opsLatency[], const InstInfo progTrace[],
                   unsigned int numOfInsts) {
        num_of_insts=numOfInsts;
        numOfInstsInt = numOfInsts;
        latencies=opsLatency;
        for (int i = 0; i <(int) numOfInsts; ++i) {
            unsigned int count = 0;
            auto *to_add = new CmndInfo(progTrace[i].isBlock,
                                        progTrace[i].dstIdx,
                                        progTrace[i].src1Idx,
                                        progTrace[i].src2Idx,
                                        progTrace[i].src3Idx,
                                        progTrace[i].opcode,
                                        progTrace[i].InstID, i);
            // cout << "i = " << i << endl;
            /// the first command has no dependencies
            for (int j = i - 1; j >= 0; j--) {
                //if block command -- all the following instructions depend on it
                if ((progTrace[i].src1Idx > 0 && (progTrace[i].src1Idx ==
                                                  progTrace[j].dstIdx)) || progTrace[j].isBlock) {//if true then there is a dependency issue
                    to_add->dependencies.push_back(insts[j]);
                    insts[j]->dependee.push_back(to_add);
                    if (!to_add->info.isBlock)
                    {
                        break;
                    }
                }
                if (to_add->info.isBlock){
                    if (progTrace[j].isBlock) break;
                    if (std::find( to_add->dependencies.begin(), to_add->dependencies.end(),insts[j]) == to_add->dependencies.end()) {
                        if(count >= Execution_units.WINDOW_SIZE*3)
                            break;
                        count++;
                        to_add->dependencies.push_back(insts[j]);
                        insts[j]->dependee.push_back(to_add);
                    }
                }
            }
            if(to_add->dependencies.size()==1){
                to_add->dependencies.push_back(nullptr);
            }
            for (int j = i - 1; j >= 0; j--) {
                //if(progTrace[i].opcode == 5) continue;
                //if block command -- all the following instructions depend on it
                if (progTrace[i].src2Idx > 0 && (progTrace[i].src2Idx ==
                                                 progTrace[j].dstIdx)) {//if true then there is a dependency issue
                    to_add->dependencies.push_back(insts[j]);
                    insts[j]->dependee.push_back(to_add);
                    break;
                }
            }
            if(to_add->dependencies.size()==2){
                to_add->dependencies.push_back(nullptr);
            }
            for (int j = i - 1; j >= 0; j--) {
                //if block command -- all the following instructions depend on it
                if (progTrace[i].src3Idx > 0 && (progTrace[i].src3Idx ==
                                                 progTrace[j].dstIdx)) {//if true then there is a dependency issue
                    to_add->dependencies.push_back(insts[j]);
                    insts[j]->dependee.push_back(to_add);
                    break;
                }
            }
            if(to_add->dependencies.size()==3){
                to_add->dependencies.push_back(nullptr);
            }
            insts.push_back(to_add);
        }
        return *this;
    }
};

ProgCtx analyzeProg(const unsigned int opsLatency[], const InstInfo progTrace[], unsigned int numOfInsts) {
    try {
        auto pp = new Commands();
        pp->Init(opsLatency, progTrace, numOfInsts);
        return pp;
    } catch (std::bad_alloc &e) {
        cout << "failed to allocate" << endl;
        return PROG_CTX_NULL;
    }
}

void freeProgCtx(ProgCtx ctx) {
    auto pp = (Commands *) ctx;
    delete pp;
}

int getInstDepthAux(ProgCtx ctx, unsigned int theInst,unsigned int* begin, int time_slot, vector<pair<unsigned int, unsigned int>> *finished_exec) {
    if(!ctx)
        return -1;
    auto pp = (Commands *) ctx;
    if(theInst>=pp->num_of_insts)
        return -1;
    vector<CmndInfo *> depends = (pp->insts)[theInst - *begin]->dependencies;
    if(!depends[0] && !depends[1] && !depends[2] && !depends[3]){
        return pp->latencies[pp->insts[theInst - *begin]->info.opcode];
    }

    int max_time = 0;
    for (auto it = depends.begin(); ++it != depends.end();) {
        if(*it) {
            if((*it)->inst_num < * begin) continue;
            int tmp = getInstDepthAux(ctx, (*it)->inst_num,begin, time_slot, finished_exec);
            if (tmp > max_time) {
                max_time = tmp;
            }
        }
    }
    return pp->latencies[pp->insts[theInst - *begin]->info.opcode]+max_time;
}

int getInstDepth(ProgCtx ctx, unsigned int theInst) {
    unsigned int begin = 0;
    vector<pair<unsigned int, unsigned int>> finished_exec;
    int max=getInstDepthAux(ctx,theInst, &begin,-1,&finished_exec);
    auto pp = (Commands *) ctx;
    return max-pp->latencies[pp->insts[theInst]->info.opcode];
}

int getInstDeps(ProgCtx ctx, unsigned int theInst, int *src1DepInst, int *src2DepInst, int *src3DepInst) {
    if(!ctx)
        return -1;
    auto pp = (Commands *) ctx;
    if(theInst>=pp->num_of_insts)
        return -1;
    *src1DepInst=-1;
    *src2DepInst=-1;
    *src3DepInst=-1;
    vector<CmndInfo*> depends=pp->insts[theInst]->dependencies;
    int i=1;
    for (auto it=depends.begin(); ++it !=depends.end();) {
        if(i==1) {
            if (*it) {
                *src1DepInst = (*it)->inst_num;
            }
        }
        if(i==2) {
            if (*it) {
                *src2DepInst = (*it)->inst_num;
            }
        }
        if(i==3) {
            if (*it) {
                *src3DepInst = (*it)->inst_num;
            }
        }
        i++;
    }
    return 0;
}

int getProgDepth_OOO(ProgCtx ctx,unsigned int* begin,int time_slot, vector<pair<unsigned int, unsigned int>> *finished_exec) {
    if(!ctx)
        return -1;
    auto pp = (Commands *) ctx;
    int max=0;
    for (unsigned int i = *begin; i < pp->num_of_insts + *begin ; ++i) {
        int tmp=getInstDepthAux(ctx, i,begin, time_slot, finished_exec);
        if(tmp>max){
            max=tmp;
        }
    }
    return max;
}

bool find(vector<pair<unsigned int, unsigned int>> vec, unsigned int num){
    for(unsigned int i =0; i < vec.size(); i++){
        if(vec[i].first == num) return true;
    }
    return false;
}


unsigned int getMaxTime(vector<pair<unsigned int, unsigned int>> vec){
    unsigned int max =0;
    for(unsigned int i=0; i<vec.size(); i++){
        if(vec[i].second > max)
            max = vec[i].second;
    }
    return max;
}

int getProgDepth_OUT_OF_ORDER(ProgCtx ctx, unsigned int* begin, vector<pair<unsigned int, unsigned int>> *finished_exec, double* alu_utilization, double* fp_utilization, double* load_utilization, double* store_utilization, double* br_utilization) {
    if(!ctx)
        return -1;
    auto pp = (Commands*) ctx;
    pair<int,int> ini(0,-1);
    vector<pair<int,int>> alu(Execution_units.ALU_UNITS,ini);
    vector<pair<int,int>> fp(Execution_units.FP_UNITS,ini);
    vector<pair<int,int>> load(Execution_units.LOAD_UNITS,ini);
    vector<pair<int,int>> store(Execution_units.STORE_UNITS,ini);
    vector<pair<int,int>> br(Execution_units.BRANCH_UNITS,ini);
    vector<unsigned int> Execute_insts;
    unsigned int max_indx = *begin;
    bool flag_end = false;
    double alu_utili = 0, fp_utili = 0, load_utili = 0, store_utili = 0, br_utili = 0;
    for(unsigned int time =0; ; time++){
        int used_alu = 0, used_fp = 0, used_load = 0, used_store = 0, used_br = 0;
        for (unsigned int i = *begin; i < pp->num_of_insts; ++i) {
            if(flag_end) break;
            if(std::count(Execute_insts.begin(),Execute_insts.end(),i) != 0) continue;
            vector<CmndInfo *> depends = pp->insts[i-*begin]->dependencies;
            int sr1 = 0, sr2 = 0, sr3 = 0;
            if (getInstDeps(ctx, i- *begin, &sr1, &sr2, &sr3) > -1) {
                if ((find(*finished_exec,sr1) || sr1 == -1 || sr1 <(int) *begin) && (find(*finished_exec,sr2) || sr2 == -1 || sr2 < (int)*begin)
                    && (find(*finished_exec,sr3) || sr3 == -1 || sr3 < (int)*begin)) {
                    bool flag = false;
                    switch (pp->insts[i-*begin]->info.opcode) {
                        case CMD_ALU: {
                            for (unsigned int alu_unit = 0; alu_unit < Execution_units.ALU_UNITS; alu_unit++) {
                                if (alu[alu_unit] == ini ) {
                                    alu[alu_unit] = pair<int,int> ((int)pp->latencies[CMD_ALU], i);
                                    //cout << "exec inst "<<i<<endl;
                                    Execute_insts.push_back(i);
                                    break;
                                } else {
                                    if (alu_unit == Execution_units.ALU_UNITS - 1) {
                                        max_indx = i;
                                        flag = true;
                                        break;
                                    }
                                }
                            }
                            if(flag) break;
                        }break;
                        case CMD_FP: {
                            for (unsigned int fp_unit = 0; fp_unit < Execution_units.FP_UNITS; fp_unit++) {
                                if (fp[fp_unit] == ini ) {
                                    fp[fp_unit] = pair<int,int> ((int)pp->latencies[CMD_FP], i);
                                    //cout << "exec inst "<<i<<endl;
                                    Execute_insts.push_back(i);
                                    break;
                                } else {
                                    if (fp_unit == Execution_units.FP_UNITS - 1) {
                                        max_indx = i;
                                        flag = true;
                                        break;
                                    }
                                }
                            }
                            if(flag) break;
                        }break;
                        case CMD_LOAD: {
                            for (unsigned int load_unit = 0; load_unit < Execution_units.LOAD_UNITS; load_unit++) {
                                if (load[load_unit] == ini) {
                                    load[load_unit] = pair<int,int>((int)pp->latencies[CMD_LOAD], i);
                                    //cout << "exec inst "<<i<<endl;
                                    Execute_insts.push_back(i);
                                    break;
                                } else {
                                    if (load_unit == Execution_units.LOAD_UNITS - 1) {
                                        max_indx = i;
                                        flag = true;
                                        break;
                                    }
                                }
                            }
                            if(flag) break;
                        }break;
                        case CMD_STORE: {
                            for (unsigned int store_unit = 0; store_unit < Execution_units.STORE_UNITS; store_unit++) {
                                if (store[store_unit] == ini ) {
                                    store[store_unit] = pair<int,int> ((int)pp->latencies[CMD_STORE], i);
                                    //cout << "exec inst "<<i<<endl;
                                    Execute_insts.push_back(i);
                                    break;
                                } else {
                                    if (store_unit == Execution_units.STORE_UNITS - 1) {
                                        max_indx = i;
                                        flag = true;
                                        break;
                                    }
                                }
                            }
                            if(flag) break;
                        }break;
                        case CMD_BRANCH: {
                            for (unsigned int br_unit = 0; br_unit < Execution_units.BRANCH_UNITS; br_unit++) {
                                if (br[br_unit] == ini) {
                                    br[br_unit] = pair<int,int>((int)pp->latencies[CMD_BRANCH], i);
                                    //cout << "exec inst "<<i<<endl;
                                    Execute_insts.push_back(i);
                                    break;
                                } else {
                                    if (br_unit == Execution_units.BRANCH_UNITS - 1) {
                                        max_indx = i;
                                        flag = true;
                                        break;
                                    }
                                }
                            }
                            if(flag) break;
                        }break;
                        default: {
                            //cout << "exec inst "<<i<<endl;
                            Execute_insts.push_back(i);
                            finished_exec->emplace_back(i,time);
                            continue;
                        }
                    }
                    //if(flag) break;
                } else {
                    continue;
                }
            }
        }
        for(unsigned int alu_unit=0; alu_unit < Execution_units.ALU_UNITS; alu_unit++){
            if(alu[alu_unit] != ini) {
                used_alu++;
                alu[alu_unit].first--;
                if (alu[alu_unit].first == 0){
                    finished_exec->emplace_back(alu[alu_unit].second,time+1);
                    alu[alu_unit] = ini;
                }
            }
        }
        for(unsigned int fp_unit=0; fp_unit < Execution_units.FP_UNITS; fp_unit++){
            if(fp[fp_unit] != ini) {
                used_fp++;
                fp[fp_unit].first--;
                if (fp[fp_unit].first == 0){
                    finished_exec->emplace_back(fp[fp_unit].second,time+1);
                    fp[fp_unit] = ini;
                }
            }
        }
        for(unsigned int load_unit=0; load_unit < Execution_units.LOAD_UNITS; load_unit++){

            if(load[load_unit] != ini) {
                used_load++;
                load[load_unit].first --;
                if(load[load_unit].first == 0){
                    finished_exec->emplace_back(load[load_unit].second, time+1);
                    load[load_unit] = ini;

                }
            }
        }
        for(unsigned int store_unit=0; store_unit < Execution_units.STORE_UNITS; store_unit++){

            if(store[store_unit] != ini) {
                used_store++;
                store[store_unit].first --;
                if(store[store_unit].first == 0){
                    finished_exec->emplace_back(store[store_unit].second, time+1);
                    store[store_unit] = ini;
                }
            }
        }
        for(unsigned int br_unit=0; br_unit < Execution_units.BRANCH_UNITS; br_unit++){

            if(br[br_unit] != ini) {
                used_br++;
                br[br_unit] .first--;
                if (br[br_unit] .first == 0){
                    finished_exec->emplace_back(br[br_unit].second, time+1);
                    br[br_unit] = ini;
                }
            }
        }

        alu_utili   += (double)used_alu/Execution_units.ALU_UNITS;
        fp_utili    += (double)used_fp/Execution_units.FP_UNITS;
        load_utili  += (double)used_load/Execution_units.LOAD_UNITS;
        store_utili += (double)used_store/Execution_units.STORE_UNITS;
        br_utili    += (double)used_br/Execution_units.BRANCH_UNITS;

        if(Execute_insts.size() == (pp->num_of_insts - *begin) || flag_end) {
            max_indx =  pp->num_of_insts;
            flag_end = true;
            bool flag = true;
            for(unsigned int alu_unit=0; alu_unit < Execution_units.ALU_UNITS; alu_unit++) {
                if (alu[alu_unit] != ini) flag = false;
            }
            for(unsigned int fp_unit=0; fp_unit < Execution_units.FP_UNITS; fp_unit++) {
                if (fp[fp_unit] != ini) flag = false;
            }
            for(unsigned int load_unit=0; load_unit < Execution_units.LOAD_UNITS; load_unit++) {
                if (load[load_unit] != ini) flag = false;
            }
            for(unsigned int store_unit=0; store_unit < Execution_units.STORE_UNITS; store_unit++) {
                if (store[store_unit] != ini) flag = false;
            }
            for(unsigned int br_unit=0; br_unit < Execution_units.BRANCH_UNITS; br_unit++) {
                if (br[br_unit] != ini) flag = false;
            }
            if(flag) {
                if(!std::count(Execute_insts.begin(), Execute_insts.end(), max_indx-1)) {
                    unsigned int max_time = getMaxTime(*finished_exec);
                    *alu_utilization   += alu_utili / ((double) max_time);
                    *fp_utilization    += fp_utili / ((double) max_time);
                    *load_utilization  += load_utili / ((double) max_time);
                    *store_utilization += store_utili / ((double) max_time);
                    *br_utilization    += br_utili / ((double) max_time);
                    return (int)max_time;
                } else{
                    unsigned int max_time = getMaxTime(*finished_exec);
                    *alu_utilization   += alu_utili / ((double) max_time);
                    *fp_utilization    += fp_utili / ((double) max_time);
                    *load_utilization  += load_utili / ((double) max_time);
                    *store_utilization += store_utili / ((double) max_time);
                    *br_utilization    += br_utili / ((double) max_time);
                    return (int)max_time;
                }
            }
        }
    }
}

int findMinuim(const vector<pair<int,int>>& vec){
    int saveIndex = 0 ;
    for(int i = 1; i <(int) vec.size(); i++)
    {
        if (vec[i].second > -1) {
            if (vec[saveIndex].second > vec[i].second || vec[saveIndex].second == -1) {
                saveIndex = i;
            }
        }
    }

//the smallest second value will be found at
    int smallestValue = vec[saveIndex].second;
    return smallestValue;
}
int findMinumOfFive(int a, int b ,int c ,int d, int e){
    int array[5] ={a, b, c, d, e};
    int min = array[0];
    for(int i = 1; i <5;i++){
        if(array[i] > -1 ) {
            if(min > array[i] || min == -1)
                min = array[i];
        }

    }
    return min;
}

unsigned int findMax(const vector<pair<unsigned int,unsigned int>>& vec){
    unsigned int max =0 ;
    for(auto & i : vec){
        if(i.first > max) max = i.first;
    }
    return max;
}

unsigned int findTime_forMax(const vector<pair<unsigned int,unsigned int>>& vec){
    unsigned int max = findMax(vec);
    for(auto & i : vec){
        if(i.first == max) return i.second;
    }
    //should not get here
    return 0;
}
//finds the Program's runtime in cycles if we would run it in IN ORDER superscalar chip
int getProgDepth_IN_ORDER(ProgCtx ctx, unsigned int *max, int time_slot, double* alu_utilization, double* fp_utilization, double* load_utilization, double* store_utilization, double* br_utilization) {
    if(!ctx)
        return -1;
    auto pp = (Commands*) ctx;
    pair<int,int> ini(0,-1);
    vector<pair<int,int>> alu(Execution_units.ALU_UNITS,ini);
    vector<pair<int,int>> fp(Execution_units.FP_UNITS,ini);
    vector<pair<int,int>> load(Execution_units.LOAD_UNITS,ini);
    vector<pair<int,int>> store(Execution_units.STORE_UNITS,ini);
    vector<pair<int,int>> br(Execution_units.BRANCH_UNITS,ini);
    double alu_utili = 0, fp_utili = 0, load_utili = 0, store_utili = 0, br_utili = 0;
    unsigned int max_indx = *max;
    int maxI = (int)max_indx;
    bool flag_end = false;
    vector<unsigned int> Execute_insts;
    vector<pair<unsigned int,unsigned int>> finished_exec; // first is instNum , second is finish time
    for(unsigned int time=0 ; ; time++ ) {
        int used_alu = 0, used_fp = 0, used_load = 0, used_store = 0, used_br = 0;
        for (unsigned int i = max_indx; i < pp->num_of_insts; ++i) {
            //cout << " loop " << endl;
            vector<CmndInfo *> depends = pp->insts[i-*max]->dependencies;
            int sr1 = 0, sr2 = 0, sr3 = 0;
            if (getInstDeps(ctx, i- *max, &sr1, &sr2, &sr3) > -1) {
                if(maxI == -1){
                    maxI = (int)max_indx;
                }
                if (sr1 < (int)maxI && sr2 < (int)maxI && sr3 < (int)maxI) {
                    bool flag = false;
                    switch (pp->insts[i-*max]->info.opcode) {
                        case CMD_ALU: {
                            for (unsigned int alu_unit = 0; alu_unit < Execution_units.ALU_UNITS; alu_unit++) {
                                if (alu[alu_unit] == ini ) {
                                    alu[alu_unit] = pair<int,int> ((int)pp->latencies[CMD_ALU], i);
                                    Execute_insts.push_back(i);
                                    //cout << "i = " << i << endl;
                                    break;
                                } else {
                                    if (alu_unit == Execution_units.ALU_UNITS - 1) {
                                        max_indx = i;
                                        flag = true;
                                        break;
                                    }
                                }
                            }
                            if(flag) break;
                        }break;
                        case CMD_FP: {
                            for (unsigned int fp_unit = 0; fp_unit < Execution_units.FP_UNITS; fp_unit++) {
                                if (fp[fp_unit] == ini ) {
                                    fp[fp_unit] = pair<int,int> ((int)pp->latencies[CMD_FP], i);
                                    Execute_insts.push_back(i);
                                    //cout << "i = " << i << endl;
                                    break;
                                } else {
                                    if (fp_unit == Execution_units.FP_UNITS - 1) {
                                        max_indx = i;
                                        flag = true;
                                        break;
                                    }
                                }
                            }
                            if(flag) break;
                        }break;
                        case CMD_LOAD: {
                            for (unsigned int load_unit = 0; load_unit < Execution_units.LOAD_UNITS; load_unit++) {
                                if (load[load_unit] == ini) {
                                    load[load_unit] = pair<int,int>((int)pp->latencies[CMD_LOAD], i);
                                    Execute_insts.push_back(i);
                                    //cout << "i = " << i << endl;

                                    break;
                                } else {
                                    if (load_unit == Execution_units.LOAD_UNITS - 1) {
                                        max_indx = i;
                                        flag = true;
                                        break;
                                    }
                                }
                            }
                            if(flag) break;
                        }break;
                        case CMD_STORE: {
                            for (unsigned int store_unit = 0; store_unit < Execution_units.STORE_UNITS; store_unit++) {
                                if (store[store_unit] == ini ) {
                                    store[store_unit] = pair<int,int> ((int)pp->latencies[CMD_STORE], i);
                                    Execute_insts.push_back(i);
                                    //cout << "i = " << i << endl;
                                    break;
                                } else {
                                    if (store_unit == Execution_units.STORE_UNITS - 1) {
                                        max_indx = i;
                                        flag = true;
                                        break;
                                    }
                                }
                            }
                            if(flag) break;
                        }break;
                        case CMD_BRANCH: {
                            for (unsigned int br_unit = 0; br_unit < Execution_units.BRANCH_UNITS; br_unit++) {
                                if (br[br_unit] == ini) {
                                    br[br_unit] = pair<int,int>((int)pp->latencies[CMD_BRANCH], i);
                                    Execute_insts.push_back(i);
                                    //cout << "i = " << i << endl;
                                    break;
                                } else {
                                    if (br_unit == Execution_units.BRANCH_UNITS - 1) {
                                        max_indx = i;
                                        flag = true;
                                        break;
                                    }
                                }
                            }
                            if(flag) break;
                        }break;
                        default: {
                            Execute_insts.push_back(i);
                            //cout << "i = " << i << endl;
                            continue;
                        }
                    }
                    if(flag) break;
                } else {
                    int minAlu = findMinuim(alu);
                    int minFp = findMinuim(fp);
                    int minLoad = findMinuim(load);
                    int minStore = findMinuim(store);
                    int minBranch = findMinuim(br);
                    maxI = findMinumOfFive(minAlu,minFp,minLoad,minStore,minBranch);
                    max_indx = i;
                    break;
                }
            }


        }
        for(unsigned int alu_unit=0; alu_unit < Execution_units.ALU_UNITS; alu_unit++){
            if(alu[alu_unit] != ini) {
                used_alu++;
                alu[alu_unit].first--;
                if (alu[alu_unit].first == 0){
                    finished_exec.emplace_back(alu[alu_unit].second,time+1);
                    alu[alu_unit] = ini;
                }
            }
        }
        for(unsigned int fp_unit=0; fp_unit < Execution_units.FP_UNITS; fp_unit++){
            if(fp[fp_unit] != ini) {
                used_fp++;
                fp[fp_unit].first--;
                if (fp[fp_unit].first == 0){
                    finished_exec.emplace_back(fp[fp_unit].second,time+1);
                    fp[fp_unit] = ini;
                }
            }
        }
        for(unsigned int load_unit=0; load_unit < Execution_units.LOAD_UNITS; load_unit++){

            if(load[load_unit] != ini) {
                used_load++;
                load[load_unit].first --;
                if(load[load_unit].first == 0){
                    finished_exec.emplace_back(load[load_unit].second, time+1);
                    load[load_unit] = ini;

                }
            }
        }
        for(unsigned int store_unit=0; store_unit < Execution_units.STORE_UNITS; store_unit++){

            if(store[store_unit] != ini) {
                used_store++;
                store[store_unit].first --;
                if(store[store_unit].first == 0){
                    finished_exec.emplace_back(store[store_unit].second, time+1);
                    store[store_unit] = ini;
                }
            }
        }
        for(unsigned int br_unit=0; br_unit < Execution_units.BRANCH_UNITS; br_unit++){

            if(br[br_unit] != ini) {
                used_br++;
                br[br_unit] .first--;
                if (br[br_unit] .first == 0){
                    finished_exec.emplace_back(br[br_unit].second, time+1);
                    br[br_unit] = ini;
                }
            }
        }

        alu_utili   += (double)used_alu/Execution_units.ALU_UNITS;
        fp_utili    += (double)used_fp/Execution_units.FP_UNITS;
        load_utili  += (double)used_load/Execution_units.LOAD_UNITS;
        store_utili += (double)used_store/Execution_units.STORE_UNITS;
        br_utili    += (double)used_br/Execution_units.BRANCH_UNITS;

        int minAlu = findMinuim(alu);
        int minFp = findMinuim(fp);
        int minLoad = findMinuim(load);
        int minStore = findMinuim(store);
        int minBranch = findMinuim(br);
        maxI = findMinumOfFive(minAlu,minFp,minLoad,minStore,minBranch);
        if((int)time == (time_slot -1)) {
            for(unsigned int & Execute_inst : Execute_insts){
                bool found = false;
                for(auto & instruction : finished_exec){
                    if(instruction.first == Execute_inst) found = true;
                }
                if(!found) {
                    *max = Execute_inst;
                    unsigned int max_time = findTime_forMax(finished_exec);
                    *alu_utilization = alu_utili / ((int)max_time);
                    *fp_utilization = fp_utili / ((int)max_time);
                    *load_utilization = load_utili / ((int)max_time);
                    *store_utilization = store_utili / ((int)max_time);
                    *br_utilization = br_utili / ((int)max_time);
                    return (int)max_time;
                } else {
                    *max = findMax(finished_exec);
                    unsigned int max_time = findTime_forMax(finished_exec);
                    *alu_utilization = alu_utili / ((int)max_time);
                    *fp_utilization = fp_utili / ((int)max_time);
                    *load_utilization = load_utili / ((int)max_time);
                    *store_utilization = store_utili / ((int)max_time);
                    *br_utilization = br_utili / ((int)max_time);
                    return (int)max_time;
                }
            }
        }
        if(Execute_insts.size() == (pp->num_of_insts - *max) || flag_end) {
            max_indx =  pp->num_of_insts;
            flag_end = true;
            bool flag = true;
            for(unsigned int alu_unit=0; alu_unit < Execution_units.ALU_UNITS; alu_unit++) {
                if (alu[alu_unit] != ini) flag = false;
            }
            for(unsigned int fp_unit=0; fp_unit < Execution_units.FP_UNITS; fp_unit++) {
                if (fp[fp_unit] != ini) flag = false;
            }
            for(unsigned int load_unit=0; load_unit < Execution_units.LOAD_UNITS; load_unit++) {
                if (load[load_unit] != ini) flag = false;
            }
            for(unsigned int store_unit=0; store_unit < Execution_units.STORE_UNITS; store_unit++) {
                if (store[store_unit] != ini) flag = false;
            }
            for(unsigned int br_unit=0; br_unit < Execution_units.BRANCH_UNITS; br_unit++) {
                if (br[br_unit] != ini) flag = false;
            }
            if(flag) {
                if(!std::count(Execute_insts.begin(), Execute_insts.end(), max_indx-1)) {
                    *max = findMax(finished_exec);
                    int max_time = (int) time + 1 + (int) pp->latencies[pp->insts[max_indx - 1- *max]->info.opcode];
                    *alu_utilization = alu_utili / ((int)max_time);
                    *fp_utilization = fp_utili / ((int)max_time);
                    *load_utilization = load_utili / ((int)max_time);
                    *store_utilization = store_utili / ((int)max_time);
                    *br_utilization = br_utili / ((int)max_time);
                    return max_time;
                } else{
                    *max = findMax(finished_exec);
                    *alu_utilization = alu_utili / ((int)time+1);
                    *fp_utilization = fp_utili / ((int)time+1);
                    *load_utilization = load_utili / ((int)time+1);
                    *store_utilization = store_utili / ((int)time+1);
                    *br_utilization = br_utili / ((int)time+1);
                    return (int) time + 1;
                }
            }
        }
    }
}
//for Round Robin Only -- calculate how much time a single window takes in a single thread
double get_ThreadWindow_IPC(ProgCtx ctx, unsigned int *begin, int windowSize, int time_slot, double* alu_utilization, double* fp_utilization, double* load_utilization, double* store_utilization, double* br_utilization){
    auto* INST_WINDOW = new Commands();
    auto pp = (Commands*) ctx;
    INST_WINDOW->latencies = pp->latencies;
    INST_WINDOW->num_of_insts = *begin + windowSize;
    Commands block_insts;
    for (unsigned int i = *begin; i < *begin + windowSize; i++) {
        if(pp->insts[i]->info.isBlock) {
            pp->insts[i]->dependencies.insert(pp->insts[i]->dependencies.end(), INST_WINDOW->insts.begin(),
                                              INST_WINDOW->insts.end());
            block_insts.insts.push_back(pp->insts[i]);
        } else {
            pp->insts[i]->dependencies.insert(pp->insts[i]->dependencies.end(), block_insts.insts.begin(), block_insts.insts.end());
        }
        INST_WINDOW->insts.push_back(pp->insts[i]);
    }

    int end = (int)*begin + (int)Execution_units.WINDOW_SIZE -1 ;
    //cout << " hh" << endl;
    double alu_utili = 0, fp_utili = 0, load_utili = 0, store_utili = 0, br_utili = 0;
    int cycles = getProgDepth_IN_ORDER(INST_WINDOW, begin, time_slot, &alu_utili, &fp_utili, &load_utili, &store_utili, &br_utili);
    int instructions = (int)Execution_units.WINDOW_SIZE- (int)(end - *begin);
    if(time_slot == -1) { time_slot = cycles;}
        *alu_utilization += (cycles *alu_utili)/time_slot;
        *fp_utilization += (cycles *fp_utili)/time_slot;
        *load_utilization += (cycles *load_utili)/time_slot;
        *store_utilization += (cycles *store_utili)/time_slot;
        *br_utilization += (cycles *br_utili)/time_slot;


    double IPC = (double)instructions/time_slot;
    delete INST_WINDOW;
    return IPC;
}
// calculate the thread ipc if we would run it on IN ORDER superscalar chip which has a limited window size

double get_MT_IPC_RR_IN_ORDER(ProgCtx *threads, double* alu_utilization, double* fp_utilization, double* load_utilization, double* store_utilization, double* br_utilization){
    double IPC = 0;
    vector<pair<ProgCtx, unsigned int>> Threads;
    for(unsigned int j=0; j < Execution_units.THREAD_NUM; j++){
        Threads.emplace_back((Commands*)threads[j],0);
    }
    bool flag = true;
    double counter = 0;
    while(flag){
        bool flag2 = true;
        for(auto & Thread : Threads){
            if(Thread.second > ((Commands*)Thread.first)->num_of_insts -1) {
                flag2 = false;
                continue;
            }
            flag2 = true;
            counter++;
            unsigned int begin = Thread.second;
            if(((Commands*)Thread.first)->num_of_insts - begin  < Execution_units.WINDOW_SIZE) {
                IPC += get_ThreadWindow_IPC(Thread.first, &begin,(int) (((Commands*)Thread.first)->num_of_insts - begin ),
                                            (int) Execution_units.slot, alu_utilization, fp_utilization, load_utilization, store_utilization, br_utilization);
            } else {
                IPC += get_ThreadWindow_IPC(Thread.first, &begin, (int) Execution_units.WINDOW_SIZE,
                                            (int) Execution_units.slot, alu_utilization, fp_utilization, load_utilization, store_utilization, br_utilization);
            }
            Thread.second = begin + 1;
            //printf("IPC = %f  begin = %d\n",IPC, begin);

        }

        if(!flag2) flag = false;
    }

    *alu_utilization = *alu_utilization / counter;
    *fp_utilization = *fp_utilization / counter;
    *load_utilization = *load_utilization / counter;
    *store_utilization = *store_utilization / counter;
    *br_utilization = *br_utilization / counter;
    return IPC / counter;
}

pair<unsigned int, unsigned int> findMin_after(vector<pair<unsigned int, unsigned int>> vec, unsigned int min, unsigned int max){
    unsigned int minimum = max + 1;
    unsigned int indx = 0;
    for(unsigned int i=0; i < vec.size() ; i++){
        if(vec[i].first < minimum && vec[i].first > min){
            minimum = vec[i].first;
            indx = i;
        }
    }

    return (vec[indx]);
}

pair<unsigned int, unsigned int> findMin(vector<pair<unsigned int, unsigned int>> vec, unsigned int max){
    unsigned int min = max;
    unsigned int indx =0;
    for(unsigned int i=0; i<vec.size(); i++){
        if(vec[i].first < min) {
            min = vec[i].first;
            indx = i;
        }
    }
    return (vec[indx]);
}

void sort(vector<pair<unsigned int, unsigned int>> &vec, unsigned int max){
    vector<pair<unsigned int, unsigned int>> sorted(vec.size());
    unsigned int min =0;
    for(unsigned int i=0; i < sorted.size(); i++){
        if(i==0) {
            sorted[i] = findMin(vec, max);
        }else {
            sorted[i] = findMin_after(vec, min, max);
        }
        min = sorted[i].first;
    }
    vec = sorted;
}

unsigned int getMax(const vector<pair<unsigned int, unsigned int>>& vec){
    unsigned int max = 0;
    for(auto & i : vec){
        if(i.first > max) max = i.first;
    }
    return max;
}

unsigned int getMax_INDEX(vector<pair<unsigned int, unsigned int>> vec, double* alu_utilization, double* fp_utilization, double* load_utilization, double* store_utilization, double* br_utilization, Commands* INST_WINDOW, unsigned int* begin, int time_slot){
    unsigned int max_time = 0;
    int used_alu = 0, used_fp = 0, used_load = 0, used_store = 0, used_br = 0;
    sort(vec,getMax(vec));
    for(unsigned int i=0; i < vec.size(); i++){
        if((int)vec[i].second <= Execution_units.slot || time_slot == -1) {
            if(vec[i].second > max_time) max_time = vec[i].second;
            if(INST_WINDOW->insts[vec[i].first - *begin]->info.opcode == CMD_ALU)
                used_alu += (int)INST_WINDOW->latencies[INST_WINDOW->insts[vec[i].first - *begin]->info.opcode];
            if(INST_WINDOW->insts[vec[i].first - *begin]->info.opcode == CMD_FP)
                used_fp += (int)INST_WINDOW->latencies[INST_WINDOW->insts[vec[i].first - *begin]->info.opcode];
            if(INST_WINDOW->insts[vec[i].first - *begin]->info.opcode == CMD_LOAD)
                used_load += (int)INST_WINDOW->latencies[INST_WINDOW->insts[vec[i].first - *begin]->info.opcode];
            if(INST_WINDOW->insts[vec[i].first - *begin]->info.opcode == CMD_STORE)
                used_store += (int)INST_WINDOW->latencies[INST_WINDOW->insts[vec[i].first - *begin]->info.opcode];
            if(INST_WINDOW->insts[vec[i].first - *begin]->info.opcode == CMD_BRANCH)
                used_br += (int)INST_WINDOW->latencies[INST_WINDOW->insts[vec[i].first - *begin]->info.opcode];
            continue;
        }
        if(time_slot != -1 && (int)max_time < time_slot) max_time = time_slot;
        *alu_utilization += (double)used_alu / (Execution_units.ALU_UNITS * max_time);
        *fp_utilization += (double)used_fp / (Execution_units.FP_UNITS * max_time);
        *load_utilization += (double)used_load / (Execution_units.LOAD_UNITS * max_time);
        *store_utilization += (double)used_store / (Execution_units.STORE_UNITS * max_time);
        *br_utilization += (double)used_br / (Execution_units.BRANCH_UNITS * max_time);

        return (int)(vec[i-1].first);
    }

    if(time_slot != -1){ max_time = time_slot;}
    *alu_utilization += (double)used_alu / (Execution_units.ALU_UNITS * max_time);
    *fp_utilization += (double)used_fp / (Execution_units.FP_UNITS * max_time);
    *load_utilization += (double)used_load / (Execution_units.LOAD_UNITS * max_time);
    *store_utilization += (double)used_store / (Execution_units.STORE_UNITS * max_time);
    *br_utilization += (double)used_br / (Execution_units.BRANCH_UNITS * max_time);
    return vec[vec.size()-1].first;
}

double get_ThreadWindow_IPC_OOO(ProgCtx ctx,unsigned int* begin,int windowSize, int time_slot, double* alu_utilization, double* fp_utilization, double* load_utilization, double* store_utilization, double* br_utilization){
    auto* INST_WINDOW = new Commands();
    auto pp = (Commands*) ctx;
    INST_WINDOW->latencies = pp->latencies;
    INST_WINDOW->num_of_insts = *begin + windowSize;
    Commands block_insts;
    for (unsigned int i = *begin; i < *begin + windowSize; i++) {
        if(pp->insts[i]->info.isBlock) {
            pp->insts[i]->dependencies.insert(pp->insts[i]->dependencies.end(), INST_WINDOW->insts.begin(),
                                              INST_WINDOW->insts.end());
            block_insts.insts.push_back(pp->insts[i]);
        } else {
            pp->insts[i]->dependencies.insert(pp->insts[i]->dependencies.end(), block_insts.insts.begin(), block_insts.insts.end());
        }
        INST_WINDOW->insts.push_back(pp->insts[i]);
    }

    vector<pair<unsigned int, unsigned int>> finished_exec; // first is inst, second is finish time
    unsigned int start = *begin;
    int end = (int) *begin + (int)Execution_units.WINDOW_SIZE -1;
    double alu_utili = 0, fp_utili =0, load_utili = 0, store_utili = 0, br_utili = 0 ;
    int cycles = getProgDepth_OUT_OF_ORDER(INST_WINDOW, begin, &finished_exec, &alu_utili, &fp_utili, &load_utili, &store_utili, &br_utili);
    *begin = getMax_INDEX(finished_exec, alu_utilization,fp_utilization, load_utilization, store_utilization,br_utilization, INST_WINDOW, &start, time_slot);
    int instructions = (int)Execution_units.WINDOW_SIZE- (int)(end - *begin);
    if(time_slot == -1) time_slot = cycles;
    double IPC = (double)instructions/time_slot;
    delete INST_WINDOW;
    return IPC;

}

double get_MT_IPC_RR_OOO(ProgCtx * threads, double* alu_utilization, double* fp_utilization, double* load_utilization, double* store_utilization, double* br_utilization){
    double IPC = 0;
    vector<pair<ProgCtx, unsigned int>> Threads;
    for(unsigned int j=0; j < Execution_units.THREAD_NUM; j++){
        Threads.emplace_back((Commands*)threads[j],0);
    }

    bool flag = true;
    double counter = 0;
    while(flag){
        bool flag2 = true;
        for(auto & Thread : Threads){
            if(Thread.second > ((Commands*)Thread.first)->num_of_insts -1) {
                flag2 = false;
                continue;
            }
            flag2 = true;
            counter++;
            unsigned int begin = Thread.second;
            if(((Commands*)Thread.first)->num_of_insts - begin  < Execution_units.WINDOW_SIZE) {
                IPC += get_ThreadWindow_IPC_OOO(Thread.first, &begin,(int) (((Commands*)Thread.first)->num_of_insts - begin ),
                                                (int) Execution_units.slot, alu_utilization, fp_utilization, load_utilization, store_utilization, br_utilization);
            } else {
                IPC += get_ThreadWindow_IPC_OOO(Thread.first, &begin, (int) Execution_units.WINDOW_SIZE,
                                                (int) Execution_units.slot, alu_utilization, fp_utilization, load_utilization, store_utilization, br_utilization);
            }
            Thread.second = begin + 1;
            //printf("IPC = %f  begin = %d\n",IPC, begin);
        }

        if(!flag2) flag = false;
    }

    *alu_utilization = *alu_utilization / (double)counter;
    *fp_utilization  = *fp_utilization / (double)counter;
    *load_utilization = *load_utilization / (double)counter;
    *store_utilization = *store_utilization / (double)counter;
    *br_utilization = *br_utilization / (double)counter;
    return IPC / (double)counter;

}

int get_window_cycles(ProgCtx ctx, unsigned int *begin, int windowSize, int time_slot) {
    auto *INST_WINDOW = new Commands();
    auto pp = (Commands *) ctx;
    INST_WINDOW->latencies = pp->latencies;
    INST_WINDOW->num_of_insts = *begin + windowSize;
    Commands block_insts;
    for (unsigned int i = *begin; i < *begin + windowSize; i++) {
        if(pp->insts[i]->info.isBlock) {
            pp->insts[i]->dependencies.insert(pp->insts[i]->dependencies.end(), INST_WINDOW->insts.begin(),
                                              INST_WINDOW->insts.end());
            block_insts.insts.push_back(pp->insts[i]);
        } else {
            pp->insts[i]->dependencies.insert(pp->insts[i]->dependencies.end(), block_insts.insts.begin(), block_insts.insts.end());
        }
        INST_WINDOW->insts.push_back(pp->insts[i]);
    }

    double utili = 0;
    if (Execution_units.IN_ORDER)
        return (int) getProgDepth_IN_ORDER(INST_WINDOW, begin, time_slot, &utili, &utili, &utili ,&utili, &utili);
    else {
        vector<pair<unsigned int, unsigned int>> finished_exec; // first is inst, second is finish time
        return getProgDepth_OUT_OF_ORDER(INST_WINDOW, begin, &finished_exec, &utili, &utili, &utili, &utili, &utili);
    }
}
typedef pair<ProgCtx ,unsigned int> THREAD_UNIT;

int FindMaximum(const vector<pair<THREAD_UNIT, int>>& vec) {
    int max = -1;
    for(auto & i : vec){
        if(i.second > max) {
            max = i.second;
        }
    }
    return max;
}

pair<THREAD_UNIT ,int> FindMin(vector<pair<THREAD_UNIT, int>> vec, int max) {
    int min = max;
    unsigned int indx =0;
    for(unsigned int i=0; i<vec.size(); i++){
        if(vec[i].second < min) {
            min = vec[i].second;
            indx = i;
        }
    }
    return (vec[indx]);
}

pair<THREAD_UNIT ,int> FindMin_after( vector<pair<THREAD_UNIT ,int>> vec, int min, int max){
    int minimum = max + 1;
    unsigned int indx = 0;
    for(unsigned int i=0; i < vec.size() ; i++){
        if(vec[i].second < minimum && vec[i].second > min){
            minimum = vec[i].second;
            indx = i;
        }
    }

    return (vec[indx]);
}
void SORT(vector<pair<THREAD_UNIT ,int>>& vec, int max) {
    vector<pair<THREAD_UNIT ,int>> sorted(vec.size());
    int min =0;
    for(unsigned int i=0; i < sorted.size(); i++){
        if(i==0) {
            sorted[i] = FindMin(vec, max);
        }else {
            sorted[i] = FindMin_after(vec, min, max);
        }
        min = sorted[i].second;
    }
    for(unsigned int i=0; i < sorted.size(); i++) vec[i] = sorted[i];
}

double get_MT_IPC_SJF(ProgCtx* threads, double* alu_utilization, double* fp_utilization, double* load_utilization, double* store_utilization, double* br_utilization){
    double IPC = 0;
    vector<pair<ProgCtx, unsigned int>> Threads;

    vector<pair<THREAD_UNIT ,int>> THREAD_EXEC_TIME;
    for(unsigned int j=0; j < Execution_units.THREAD_NUM; j++){
        Threads.emplace_back((Commands*)threads[j],0);
        THREAD_EXEC_TIME.emplace_back(Threads[j],0);
    }

    bool flag = true;
    int counter = 0;
    while(flag){
        bool flag2 = true;
        for(auto & Thread : THREAD_EXEC_TIME){
            if(Thread.first.second > ((Commands*)Thread.first.first)->num_of_insts -1) {
                flag2 = false;
                Thread.second = -1;
                continue;
            }
            flag2 = true;
            counter++;
            unsigned int begin = Thread.first.second;
            if(((Commands*)Thread.first.first)->num_of_insts - begin  < Execution_units.WINDOW_SIZE) {
                Thread.second = get_window_cycles(Thread.first.first, &begin,
                                                  (int) (((Commands*)Thread.first.first)->num_of_insts - begin ), -1);
            } else {
                Thread.second = get_window_cycles(Thread.first.first, &begin,(int) Execution_units.WINDOW_SIZE, -1);
            }
        }

        SORT(THREAD_EXEC_TIME, FindMaximum(THREAD_EXEC_TIME));
        for(auto & it : THREAD_EXEC_TIME) {
            if(it.second == -1) continue;
            unsigned int begin = it.first.second;
            if(((Commands*)it.first.first)->num_of_insts - begin  < Execution_units.WINDOW_SIZE) {
                if(Execution_units.IN_ORDER)
                    IPC += get_ThreadWindow_IPC(it.first.first, &begin, (int) (((Commands*)it.first.first)->num_of_insts - begin ), -1, alu_utilization, fp_utilization, load_utilization, store_utilization, br_utilization);
                else
                    IPC += get_ThreadWindow_IPC_OOO(it.first.first, &begin, (int) (((Commands*)it.first.first)->num_of_insts - begin ), -1, alu_utilization, fp_utilization, load_utilization, store_utilization, br_utilization);
            } else {
                if(Execution_units.IN_ORDER)
                    IPC += get_ThreadWindow_IPC(it.first.first, &begin,(int) Execution_units.WINDOW_SIZE, -1, alu_utilization, fp_utilization, load_utilization, store_utilization, br_utilization);
                else
                    IPC += get_ThreadWindow_IPC_OOO(it.first.first, &begin,(int) Execution_units.WINDOW_SIZE, -1, alu_utilization, fp_utilization, load_utilization, store_utilization, br_utilization);
            }
            it.first.second = begin + 1;
            //printf("IPC = %f  begin = %d\n",IPC, begin);

        }

        if(!flag2) flag = false;
    }

    *alu_utilization = *alu_utilization / (double)counter;
    *fp_utilization  = *fp_utilization / (double)counter;
    *load_utilization = *load_utilization / (double)counter;
    *store_utilization = *store_utilization / (double)counter;
    *br_utilization = *br_utilization / (double)counter;
    return IPC / (double)counter;
}

void deleteExecutedInsDependencies(ProgCtx ctx, int index){
    auto pp =(Commands*) ctx;
    /*   for (int j = 0; j < pp->num_of_insts; ++j) {
               if(std::count(pp->insts[j]->dependencies.begin(),pp->insts[j]->dependencies.end(),pp->insts[index])){
                   pp->insts[j]->dependencies.erase(std::remove(pp->insts[j]->dependencies.begin(), pp->insts[j]->dependencies.end(), pp->insts[index]), pp->insts[j]->dependencies.end());
               }
       }
       */
    for (int j = 0; j <(int) pp->insts[index]->dependee.size(); ++j) {
        pp->insts[index]->dependee[j]->dependencies.erase(std::remove(pp->insts[index]->dependee[j]->dependencies.begin(), pp->insts[index]->dependee[j]->dependencies.end(), pp->insts[index]), pp->insts[index]->dependee[j]->dependencies.end());
    }
}

void deleteExecutedIns(ProgCtx ctx, int index, vector<pair<int,int>>& CommandsDone,int threadNum){
    // we have to delete all the dependencies of this ins first!
    auto pp =(Commands*) ctx;
    CmndInfo * it;
    int j = 0;
    for (int i = 0; i <(int) pp->numOfInstsInt; ++i) {
        if ((int)pp->insts[i]->inst_num == index){
            it = pp->insts[i];
            j = i;
            break;
        }
    }
    if(!it->info.isBlock) {
        pair<int, int> kk(index, threadNum);
        CommandsDone.erase(std::remove(CommandsDone.begin(), CommandsDone.end(), kk), CommandsDone.end());
    }
    deleteExecutedInsDependencies(ctx, j);
    //now we have to delete it from the ctx :)
    pp->insts.erase(std::remove(pp->insts.begin(), pp->insts.end(), it), pp->insts.end());
    pp->numOfInstsInt--;
}



int insWithDependencies(ProgCtx ctx,int ins){
    int count = 0;
    for (int j = 0; j < (int) Execution_units.WINDOW_SIZE*2; ++j)
    {
        int sr1 = 0, sr2 = 0, sr3 = 0;
        if (getInstDeps(ctx, j, &sr1, &sr2, &sr3) > -1) {
            if (sr1 == ins || sr2 == ins || sr3 == ins){
                count++;
            }
        }
    }

    return count;
}

bool sortBySecond(const pair<pair<int,int>,int> &a,
                  const pair<pair<int,int>,int> &b)
{
    return (a.second > b.second);
}



vector<pair<pair<int,int>,int>> sortDependenciesOfAllThreads(ProgCtx* threads,const vector<pair<pair<int,int>,int>>& insToRun){
    vector<pair<pair<int,int>,int>> sortedThreadsDependencies;

    for (int j = 0; j < (int)Execution_units.THREAD_NUM; ++j) {
        vector<pair<pair<pair<int,int>,int>,int>> SortedDependenciesForInsToRun;
        for (auto & i : insToRun) {
            if (i.first.second == j){
                int count = insWithDependencies(threads[j], i.first.first);
                pair<int,int> pair1 = make_pair(i.first.first,i.first.second);
                pair<pair<int,int>,int> pair2 = make_pair(pair1 , count);
                sortedThreadsDependencies.push_back(pair2);
            }
        }
    }
    std::sort(sortedThreadsDependencies.begin(), sortedThreadsDependencies.end(), sortBySecond);
    if(Execution_units.IN_ORDER){
        for(int j = 0; j < (int)Execution_units.THREAD_NUM; j++) {
            bool flag = false;
            for (int i = 0; i < sortedThreadsDependencies.size(); i++) {
                if (sortedThreadsDependencies[i].first.second == j)
                    for(int k = i+1; k < sortedThreadsDependencies.size();k++){
                        if(sortedThreadsDependencies[k].first.second == j){
                            if(sortedThreadsDependencies[i].first.first > sortedThreadsDependencies[k].first.first){
                                std::swap(sortedThreadsDependencies[i], sortedThreadsDependencies[k]);
                                flag = true;
                                break;
                            }
                        }
                    }
                if(flag){
                    i--;
                    flag = false;
                }
            }
        }
    }
    vector<pair<pair<int,int>,int>> sortedThreadsDependenciesFinal;
    sortedThreadsDependenciesFinal.reserve(sortedThreadsDependencies.size());
    for (auto & sortedThreadsDependencie : sortedThreadsDependencies)
    {
        int opCode;
        for(const auto & i : insToRun){
            if(i.first == sortedThreadsDependencie.first){
                opCode = i.second;
                break;
            }
        }
        pair<pair<int,int>,int> pair2(sortedThreadsDependencie.first , opCode);
        sortedThreadsDependenciesFinal.push_back(pair2);
    }
    return sortedThreadsDependenciesFinal;
}

// pair<ins_number,opcode>
vector<pair<int,int>> checkWhichInsCanRunInInsWindowInOrder(ProgCtx ctx, int begin, int ins_window, vector<pair<int, int>>& CommandsInExe, int threadNum,vector<pair<int,int>>& CommandsDone){
    auto pp =(Commands*) ctx;
    vector<pair<int,int>> canRun;
    for (int i = begin; i < begin + ins_window && i <pp->numOfInstsInt; ++i)
    {
        int count = 0;
        pair<int,int> temp(pp->insts[i]->inst_num,threadNum);
        if (std::find(CommandsInExe.begin(),CommandsInExe.end(),temp) != CommandsInExe.end()) continue;
        if (pp->insts[i]->info.isBlock && 0 != i ){
            break;
        }
        if (pp->insts[i]->info.isBlock && 0 == i ){
            pair<int,int> ins(pp->insts[i]->inst_num,pp->insts[i]->info.opcode);
            canRun.push_back(ins);
            break;
        }
        vector<CmndInfo *> depends = pp->insts[i]->dependencies;
        for(int i = 0; i < (int) depends.size(); i++) {
            if(depends[i]) // this instruction can run :)
            {
                count++;
                pair<int,int> tmp22(depends[i]->inst_num,threadNum);
                pair<int,int> tmp33(depends[i]->inst_num,depends[i]->info.opcode);
                if (std::find(canRun.begin(),canRun.end(),tmp33) != canRun.end() || std::find(CommandsInExe.begin(),CommandsInExe.end(),tmp22) != CommandsInExe.end() || std::find(CommandsDone.begin(), CommandsDone.end(), tmp22) == CommandsDone.end()) continue;
                count--;
                // we don't have to break because we are OOO.
            }
        }
        if(count == 0){
            pair<int, int> ins(pp->insts[i]->inst_num, pp->insts[i]->info.opcode);
            canRun.push_back(ins);
        }else{
            return canRun;
        }
    }
    return canRun;
}
// pair<ins_number,opcode>
vector<pair<int,int>> checkWhichInsCanRunInInsWindowOOO(ProgCtx ctx, int begin, int ins_window, vector<pair<int, int>>& CommandsInExe, int threadNum,vector<pair<int,int>>& CommandsDone){
    auto pp =(Commands*) ctx;
    vector<pair<int,int>> canRun;
    for (int i = begin; i < begin + ins_window && i < pp->numOfInstsInt; ++i)
    {
        int count = 0;
        pair<int,int> temp(pp->insts[i]->inst_num,threadNum);
        if (std::find(CommandsInExe.begin(),CommandsInExe.end(),temp) != CommandsInExe.end()) continue;
        if (pp->insts[i]->info.isBlock && 0 != i ){
            break;
        }
        if (pp->insts[i]->info.isBlock && 0 == i ){
            pair<int,int> ins(pp->insts[i]->inst_num,pp->insts[i]->info.opcode);
            canRun.push_back(ins);
            break;
        }
        vector<CmndInfo *> depends = pp->insts[i]->dependencies;
        for(int i = 0; i < (int) depends.size(); i++) {
            if(depends[i]) // this instruction can run :)
            {
                count++;
                pair<int,int> tmp22(depends[i]->inst_num,threadNum);
                pair<int,int> tmp33(depends[i]->inst_num,depends[i]->info.opcode);
                if (std::find(canRun.begin(),canRun.end(),tmp33) != canRun.end() || std::find(CommandsInExe.begin(),CommandsInExe.end(),tmp22) != CommandsInExe.end() || std::find(CommandsDone.begin(), CommandsDone.end(), tmp22) == CommandsDone.end()) continue;
                count--;
                // we don't have to break because we are OOO.
            }
        }
        if(count == 0){
            pair<int, int> ins(pp->insts[i]->inst_num, pp->insts[i]->info.opcode);
            canRun.push_back(ins);
        }
    }
    return canRun;
}


//pair<ins,thread>

vector<pair<pair<int,int>,int>> whichCanRunInFIFOFromAllThreads(ProgCtx* threads, vector<pair<int,int>>(*f)(ProgCtx, int, int,vector<pair<int, int>>& CommandsInExe,int, vector<pair<int, int>>& CommandsDone), vector<pair<int, int>>& CommandsInExe,  int vacancyInBR, int vacancyInSTORE, int vacancyInLOAD , int vacancyInFP, int vacancyInALU, vector<pair<int,int>>& CommandsDone){
    vector<pair<pair<int,int>,int>> allThreads;
    int aluIns = 0;
    int fpIns = 0;
    int loadIns = 0;
    int storeIns = 0;
    int branchIns = 0;
    for( int i=0; i < (int)Execution_units.THREAD_NUM; i++)
    {
        auto pp = (Commands *) threads[i];
        vector<pair<int, int>> canRun = f(pp, 0, (int) Execution_units.WINDOW_SIZE, CommandsInExe,i, CommandsDone);
        for (auto & j : canRun)
        {
            switch(j.second){
                case CMD_ALU:
                    aluIns++;
                    break;
                case CMD_BRANCH:
                    branchIns++;
                    break;
                case CMD_FP:
                    fpIns++;
                    break;
                case CMD_LOAD:
                    loadIns++;
                    break;
                case CMD_STORE:
                    storeIns++;
                    break;
                default:
                    break;
            }
            pair<int,int> pair1(j.first,i);
            pair<pair<int,int>,int> pair2(pair1,j.second);
            allThreads.push_back(pair2);
        }

    }
    vector<pair<pair<int,int>,int>> finalToRunCommands;
    // here i have which commands can run, now I will check if there are available units for all of these commands,
    // if yes then go :) if no :( , I have to check who will run first using my sort function!

    if(aluIns <= vacancyInALU && fpIns <= vacancyInFP && loadIns <= vacancyInLOAD && storeIns <= vacancyInSTORE && branchIns <= vacancyInBR)
    {
        // now we are happy, because all the instructions that can run has vacancy ,
        // so we can put them in Exe.
        return allThreads;
    }else
    {
        aluIns = 0;
        fpIns = 0;
        loadIns = 0;
        storeIns = 0;
        branchIns = 0;
        vector <int> inOrderThreads;
        // now we have to check which instructions have priority!
        vector<pair<pair<int,int>,int>> ofAllThreads = sortDependenciesOfAllThreads(threads, allThreads);
        for (auto & ofAllThread : ofAllThreads)
        {
            if (ofAllThread.second == CMD_ALU && !(Execution_units.IN_ORDER && std::find(inOrderThreads.begin(),inOrderThreads.end(),ofAllThread.first.second) != inOrderThreads.end()))
            {
                if (aluIns < vacancyInALU){
                    aluIns++;
                    finalToRunCommands.push_back(ofAllThread);
                } else{
                    if(Execution_units.IN_ORDER ) {
                        if(std::find(inOrderThreads.begin(),inOrderThreads.end(),ofAllThread.first.second) == inOrderThreads.end()) {
                            inOrderThreads.push_back(ofAllThread.first.second);
                        }
                    }
                }
            }
            if (ofAllThread.second == CMD_FP && !(Execution_units.IN_ORDER &&std::find(inOrderThreads.begin(),inOrderThreads.end(),ofAllThread.first.second) != inOrderThreads.end()))
            {
                if (fpIns < vacancyInFP){
                    fpIns++;
                    finalToRunCommands.push_back(ofAllThread);
                }else{
                    if(Execution_units.IN_ORDER ) {
                        if(std::find(inOrderThreads.begin(),inOrderThreads.end(),ofAllThread.first.second) == inOrderThreads.end()) {
                            inOrderThreads.push_back(ofAllThread.first.second);
                        }
                    }
                }
            }
            if (ofAllThread.second == CMD_LOAD && !(Execution_units.IN_ORDER && std::find(inOrderThreads.begin(),inOrderThreads.end(),ofAllThread.first.second) != inOrderThreads.end()))
            {
                if (loadIns < vacancyInLOAD){
                    loadIns++;
                    finalToRunCommands.push_back(ofAllThread);
                }else{
                    if(Execution_units.IN_ORDER ) {
                        if(std::find(inOrderThreads.begin(),inOrderThreads.end(),ofAllThread.first.second) == inOrderThreads.end()) {
                            inOrderThreads.push_back(ofAllThread.first.second);
                        }
                    }
                }
            }
            if (ofAllThread.second == CMD_STORE && !(Execution_units.IN_ORDER && std::find(inOrderThreads.begin(),inOrderThreads.end(),ofAllThread.first.second) != inOrderThreads.end()))
            {
                if (storeIns < vacancyInSTORE){
                    storeIns++;
                    finalToRunCommands.push_back(ofAllThread);
                }else{
                    if(Execution_units.IN_ORDER ) {
                        if(std::find(inOrderThreads.begin(),inOrderThreads.end(),ofAllThread.first.second) == inOrderThreads.end()) {
                            inOrderThreads.push_back(ofAllThread.first.second);
                        }
                    }
                }
            }
            if (ofAllThread.second == CMD_BRANCH && !(Execution_units.IN_ORDER && std::find(inOrderThreads.begin(),inOrderThreads.end(),ofAllThread.first.second) != inOrderThreads.end()))
            {
                if (branchIns < vacancyInBR){
                    branchIns++;
                    finalToRunCommands.push_back(ofAllThread);
                }else{
                    if(Execution_units.IN_ORDER ) {
                        if(std::find(inOrderThreads.begin(),inOrderThreads.end(),ofAllThread.first.second) == inOrderThreads.end()) {
                            inOrderThreads.push_back(ofAllThread.first.second);
                        }
                    }
                }
            }
            if (ofAllThread.second == CMD_NOP && !(Execution_units.IN_ORDER && std::find(inOrderThreads.begin(),inOrderThreads.end(),ofAllThread.first.second) != inOrderThreads.end()))
            {
                finalToRunCommands.push_back(ofAllThread);
            }

        }
    }
    return finalToRunCommands;
}
void CommandsToExe(ProgCtx * threads, vector<pair<pair<int,int>,int>>& commandsToRun, vector<pair<pair<int,int>,int>>& ALU,  vector<pair<pair<int,int>,int>>& FP,  vector<pair<pair<int,int>,int>>& BR,  vector<pair<pair<int,int>,int>>& STORE,  vector<pair<pair<int,int>,int>>& LOAD, vector<pair<int,int>>& NOP,  int* vacancyInBR, int* vacancyInSTORE, int* vacancyInLOAD , int* vacancyInFP, int* vacancyInALU){
    for (auto & j : commandsToRun) {
        auto pp = (Commands *) threads[j.first.second];
        switch(j.second){
            case CMD_ALU: {
                (*vacancyInALU)--;
                for (auto & i : ALU) {
                    if (i.second == 0) {
                        i.second = pp->latencies[CMD_ALU];
                        i.first = j.first;
                        break;
                    }
                }
            }break;
            case CMD_BRANCH: {
                (*vacancyInBR)--;
                for (auto & i : BR) {
                    if (i.second == 0) {
                        i.second = pp->latencies[CMD_BRANCH];
                        i.first = j.first;
                        break;
                    }
                }
            } break;
            case CMD_FP: {
                (*vacancyInFP)--;
                for (auto & i : FP) {
                    if (i.second == 0) {
                        i.second = pp->latencies[CMD_FP];
                        i.first = j.first;
                        break;
                    }
                }
            } break;
            case CMD_LOAD: {
                (*vacancyInLOAD)--;
                for (auto & i : LOAD) {
                    if (i.second == 0) {
                        i.second = pp->latencies[CMD_LOAD];
                        i.first = j.first;
                        break;
                    }
                }
            }break;
            case CMD_STORE: {
                (*vacancyInSTORE)--;
                for (auto & i : STORE) {
                    if (i.second == 0){
                        i.second= pp->latencies[CMD_STORE];
                        i.first = j.first;
                        break;
                    }
                }
            }break;
            default: {
                NOP.push_back(j.first);
            } break;
        }
    }
}

vector<pair<pair<int,int>,int>> RetireFromExe( vector<pair<pair<int,int>,int>>& ALU,  vector<pair<pair<int,int>,int>>& FP,  vector<pair<pair<int,int>,int>>& BR,  vector<pair<pair<int,int>,int>>& STORE,  vector<pair<pair<int,int>,int>>& LOAD, vector<pair<int,int>>& NOP, int* vacancyInBR, int* vacancyInSTORE, int* vacancyInLOAD , int* vacancyInFP, int* vacancyInALU){
    pair<int,int> INI(-1,-1);
    vector<pair<pair<int,int>,int>> done;
    for (auto & i : ALU) {
        if (i.first != INI) {
            if (i.second != 0) {
                i.second--;
            }
            if(i.second == 0){
                (*vacancyInALU)++;
                pair<pair<int,int>,int> pair1(i.first,CMD_ALU);
                done.push_back(pair1);
                i.first = INI;
            }
        }
    }
    for (auto & i : FP) {
        if (i.first != INI) {
            if (i.second != 0) {
                i.second--;
            }
            if(i.second == 0){
                pair<pair<int,int>,int> pair1(i.first,CMD_FP);
                done.push_back(pair1);
                (*vacancyInFP)++;
                i.first = INI;
            }
        }
    }
    for (auto & i : LOAD) {
        if (i.first != INI) {
            if (i.second != 0) {
                i.second--;
            }
            if(i.second == 0){
                pair<pair<int,int>,int> pair1(i.first,CMD_LOAD);
                done.push_back(pair1);
                (*vacancyInLOAD)++;
                i.first = INI;
            }
        }
    }
    for (auto & i : STORE) {
        if (i.first != INI) {
            if (i.second != 0) {
                i.second--;
            }
            if(i.second == 0){
                pair<pair<int,int>,int> pair1(i.first,CMD_STORE);
                done.push_back(pair1);
                (*vacancyInSTORE)++;
                i.first = INI;
            }
        }
    }
    for (auto & i : BR) {
        if (i.first != INI) {
            if (i.second != 0) {
                i.second--;
            }
            if(i.second == 0){
                pair<pair<int,int>,int> pair1(i.first,CMD_BRANCH);
                done.push_back(pair1);
                (*vacancyInBR)++;
                i.first = INI;
            }
        }
    }
    for (auto & i : NOP) {
        pair<pair<int,int>,int> pair1(i, CMD_NOP);
        done.push_back(pair1);
        NOP.erase(std::remove(NOP.begin(), NOP.end(), i), NOP.end());
    }
    return done;
}

double FIFOMT(ProgCtx *threads){

    pair<int,int> INI(-1,-1);
    pair<pair<int,int>,int> INI1(INI, 0);
    int vacancyInBR =(int) Execution_units.BRANCH_UNITS;
    int vacancyInSTORE =(int) Execution_units.STORE_UNITS;
    int vacancyInLOAD =(int) Execution_units.LOAD_UNITS;
    int vacancyInFP =(int) Execution_units.FP_UNITS;
    int vacancyInALU =(int) Execution_units.ALU_UNITS;
    vector<pair<pair<int,int>,int>> ALU(Execution_units.ALU_UNITS,INI1);
    vector<pair<pair<int,int>,int>> FP(Execution_units.FP_UNITS,INI1);
    vector<pair<pair<int,int>,int>> LOAD(Execution_units.LOAD_UNITS,INI1);
    vector<pair<pair<int,int>,int>> STORE(Execution_units.STORE_UNITS,INI1);
    vector<pair<pair<int,int>,int>> BR(Execution_units.BRANCH_UNITS,INI1);
    vector<pair<int, int>> CommandsInExe;
    vector<pair<int, int>> CommandsDone;
    vector<pair<int, int>> NOP;
    vector<pair<double,double>> CyclesForThread;
    vector<bool> ThreadIsDone;
    double ALUUtilization = 0;
    double FPUtilization = 0;
    double BRANCHUtilization = 0;
    double STOREUtilization = 0;
    double LOADUtilization = 0;
    double time = 0;
    bool end = false;
    double numOfInsts = 0;
    for (int i = 0; i <(int) Execution_units.THREAD_NUM; ++i) {
        auto pp = (Commands *) threads[i];
        numOfInsts += (double)pp->num_of_insts;
        pair<double,double> pair6((double)pp->num_of_insts, 0);
        CyclesForThread.push_back(pair6);
        ThreadIsDone.push_back(false);
    }
    vector<pair<pair<int,int>,int>> executedIns;
    while(true )
    {
        for (int i = 0; i <(int) Execution_units.THREAD_NUM; ++i)
        {
            auto pp = (Commands *) threads[i];
            //printf("%d\n",pp->numOfInstsInt);
            if(pp->numOfInstsInt > 0){
                break;
            }
            if(!ThreadIsDone[i]) {
                CyclesForThread[i].second = time;
                ThreadIsDone[i] = true;
            }
            if(i == (int) (Execution_units.THREAD_NUM-1) ) end = true;
        }
        if (end) break;
        executedIns = RetireFromExe(ALU, FP, BR, STORE, LOAD, NOP, &vacancyInBR, &vacancyInSTORE, &vacancyInLOAD , &vacancyInFP, &vacancyInALU);
        for (auto & executedIn : executedIns) {
            auto pp = (Commands *) threads[executedIn.first.second];
            pair <int,int> pair7(executedIn.first.first, executedIn.first.second);
            CommandsDone.insert(CommandsDone.begin(),pair7);
            CommandsInExe.erase( std::remove(CommandsInExe.begin(), CommandsInExe.end(), pair7), CommandsInExe.end());
            deleteExecutedIns(pp, executedIn.first.first,CommandsDone,executedIn.first.second);
        }
        if (Execution_units.IN_ORDER)
        {
            executedIns = whichCanRunInFIFOFromAllThreads(threads,checkWhichInsCanRunInInsWindowInOrder,CommandsInExe, vacancyInBR,  vacancyInSTORE, vacancyInLOAD , vacancyInFP, vacancyInALU, CommandsDone);

        }
        else
        {
            executedIns = whichCanRunInFIFOFromAllThreads(threads,checkWhichInsCanRunInInsWindowOOO,CommandsInExe, vacancyInBR, vacancyInSTORE, vacancyInLOAD , vacancyInFP, vacancyInALU, CommandsDone);
        }
        CommandsToExe(threads, executedIns, ALU, FP, BR, STORE, LOAD, NOP, &vacancyInBR, &vacancyInSTORE, &vacancyInLOAD , &vacancyInFP, &vacancyInALU);
        for (auto & executedIn : executedIns)
        {
            CommandsInExe.push_back(executedIn.first);
            //printf(" ins = %d", (executedIn.first.first));
        }
        // now all the instructions in this instruction window are executed, so I have to delete them and go get some sleep :)!!!
        ALUUtilization = ALUUtilization + ((Execution_units.ALU_UNITS - vacancyInALU)/(double)Execution_units.ALU_UNITS);
        FPUtilization = FPUtilization + ((Execution_units.FP_UNITS - vacancyInFP)/(double)Execution_units.FP_UNITS);
        BRANCHUtilization = BRANCHUtilization + ((Execution_units.BRANCH_UNITS - vacancyInBR)/(double)Execution_units.BRANCH_UNITS);
        STOREUtilization = STOREUtilization + ((Execution_units.STORE_UNITS - vacancyInSTORE)/(double)Execution_units.STORE_UNITS);
        LOADUtilization = LOADUtilization + ((Execution_units.LOAD_UNITS - vacancyInLOAD)/(double)Execution_units.LOAD_UNITS);
        time++;
        //printf("time = %f\n",time);

    }
    printf("ALU UTILIZATION = %f\n",ALUUtilization/time);
    printf("FP UTILIZATION = %f\n",FPUtilization/time);
    printf("BRANCH UTILIZATION = %f\n",BRANCHUtilization/time);
    printf("STORE UTILIZATION = %f\n",STOREUtilization/time);
    printf("LOAD UTILIZATION = %f\n",LOADUtilization/time);
    for (int i = 0; i < (int) CyclesForThread.size(); ++i) {
        double ipc = CyclesForThread[i].first/CyclesForThread[i].second;
        printf("for thread = %d get_MT_IPC_FF()==%f\n",i, ipc);
    }
    return numOfInsts/time;
}
