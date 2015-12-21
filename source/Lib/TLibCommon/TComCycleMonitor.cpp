
#include <vector>
#include <cstdlib>
#include <fstream>
#include <cstdio>
#include <sys/time.h>
#include <sstream>
#include <assert.h>
#include "TComCycleMonitor.h"

//#include "TAppEncoder/TAppEncCfg.h"
//#include "TLibCommon/TComCycleMonitor.h"

using namespace std;

vector<dictionary> TComCycleMonitor::cycleVector;
ofstream TComCycleMonitor::cycleResults;
//uint TComCycleMonitor::currFrame;
struct timeval TComCycleMonitor::timer;
struct timespec TComCycleMonitor::ts_start;
struct timespec TComCycleMonitor::ts_end;
std::string TComCycleMonitor::currDistFunc;
bool TComCycleMonitor::isIntra;
bool TComCycleMonitor::isMerge;
bool TComCycleMonitor::isOthers;
uint TComCycleMonitor::currDepth;

TComCycleMonitor::TComCycleMonitor() {

}


void TComCycleMonitor::init(double qp){
    if(not(cycleResults.is_open())){        
        
        ostringstream file_path;
        file_path << "function_cycle_results_" << (int) qp << ".csv";
        cycleResults.open(file_path.str().c_str(), ofstream::out);
        
        cycleResults << "\t Total Time (s)\tCalls (kCalls)"<< endl;

    }
    isIntra = false;
    isMerge = false;
    isOthers = true;
    currDepth = 0;
    currDistFunc = "Null";
    cycleVector.clear();
}

dictionary TComCycleMonitor::createTriplet(string func){
    dictionary triplet;
    vector<double> ret;
    for(int i = 0; i < 3; i++)
        ret.push_back(0.0);
    triplet = make_pair(func, ret);
    return triplet;
}

void TComCycleMonitor::setCurrDepth(uint d){
    currDepth = d;
}

void TComCycleMonitor::setInitCycle(string f, uint d){
    std::string func;
    if (f[0] == 'T' and f[1] == 'R'){
        ostringstream sstr;
        sstr << f << "_" << d;
        func = sstr.str();
    }
    else
        func = f;
   // currDepth = d;
    if(func != "Others" and isOthers == true){
        isOthers = false;
        setEndCycle("Others");
    }
    
    unsigned long long begin;
    
    vector<dictionary>::iterator it;
//#if USE_RDTSC 
//    unsigned long long begin = rdtsc();
//#else
//    gettimeofday(&timer,NULL);
//#endif
     // tries to find the function in the vector, if not, create node
    for(it = cycleVector.begin(); it != cycleVector.end(); it++){
        if (it->first == func){
            
#if USE_RDTSC
            begin = rdtsc();

            it->second[0] = begin/CPU_TICKS_PER_SECOND;
#else
            gettimeofday(&timer,NULL);
            it->second[0] = timer.tv_sec +  timer.tv_usec*1.0/1000000;
#endif
            break;
        }
    }
    if(it == cycleVector.end()){
        dictionary triplet = createTriplet(func);
#if USE_RDTSC
        begin = rdtsc();

        triplet.second[0] = begin/CPU_TICKS_PER_SECOND;
#else       
        triplet.second[0] = timer.tv_sec +  timer.tv_usec*1.0/1000000;
#endif
        
        cycleVector.push_back(triplet);
    }
}

void TComCycleMonitor::setEndCycle(string f){
    
#if USE_RDTSC
    unsigned long long end = rdtsc();
#else
    gettimeofday(&timer,NULL);
#endif
    
    std::string func;

    if (f[0] == 'T' and f[1] == 'R'){
        ostringstream sstr;
        sstr << f << "_" << currDepth;
        func = sstr.str();
    }
    else
        func = f;
    vector<dictionary>::iterator it;
    double diff_time;

    for(it = cycleVector.begin(); it != cycleVector.end(); it++){
        if (it->first == func){
#if USE_RDTSC
            double end_time = end/CPU_TICKS_PER_SECOND;
#else
            double end_time = timer.tv_sec +  timer.tv_usec*1.0/1000000;
#endif
            diff_time = end_time - it->second[0];
            assert(diff_time != 0);
            it->second[2]++;
            it->second[1] += diff_time;
            break;
        }
    }
        
    if(func != "Others" and isOthers == false){
        isOthers = true;
        setInitCycle("Others", currDepth);
    }

}
void TComCycleMonitor::setDistFuncString(int w, int h, int func_enum){
    string distFunc;
    ostringstream stringst;
    
    if ( BETWEEN(func_enum, 1,7) )
        distFunc = "SSE";
    else if ( BETWEEN(func_enum, 8,21) )
        distFunc = "SAD";
    else if ( BETWEEN(func_enum, 22,28) )
        distFunc = "HAD";
    else if ( BETWEEN(func_enum, 43,48) )
        distFunc = "SAD";
    else
        distFunc = "Null";
    
    stringst << "DIST_" << distFunc << "_" << w << "x" << h;
    currDistFunc = stringst.str();
}

void TComCycleMonitor::reportCycles(int currFrame){
    vector<dictionary>::iterator it;
    
    cycleResults << "Frame\t" << currFrame << endl;
    for(it = cycleVector.begin(); it != cycleVector.end(); it++){
        cycleResults << it->first << "\t" << it->second[1]<< "\t" << it->second[2]/1000 << endl;
    }
    cycleResults << endl;
    currFrame++;
    cycleVector.clear();
}
