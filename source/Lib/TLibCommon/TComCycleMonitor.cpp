
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
uint TComCycleMonitor::currFrame;
struct timeval TComCycleMonitor::timer;
std::string TComCycleMonitor::currDistFunc;
bool TComCycleMonitor::isIntra;
bool TComCycleMonitor::isMerge;
bool TComCycleMonitor::isOthers;


TComCycleMonitor::TComCycleMonitor() {

}


void TComCycleMonitor::init(){
    if(not(cycleResults.is_open())){        
     
        
        cycleResults.open("function_cycle_results.csv", ofstream::out);
        
        cycleResults << "\t Total Time (s)\tCalls (kCalls)"<< endl;

    }
    isIntra = false;
    isMerge = false;
    isOthers = true;
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

void TComCycleMonitor::setInitCycle(string func){
    if(func != "Others" and isOthers == true){
        isOthers = false;
        setEndCycle("Others");
    }
    
    vector<dictionary>::iterator it;
    gettimeofday(&timer,NULL);
    
     // tries to find the function in the vector, if not, create node
    for(it = cycleVector.begin(); it != cycleVector.end(); it++){
        if (it->first == func){
            it->second[0] = timer.tv_sec +  timer.tv_usec*1.0/1000000;

            break;
        }
    }
    if(it == cycleVector.end()){
        dictionary triplet = createTriplet(func);
        triplet.second[0] = timer.tv_sec +  timer.tv_usec*1.0/1000000;
        
        cycleVector.push_back(triplet);
    }
}

void TComCycleMonitor::setEndCycle(string func){
    vector<dictionary>::iterator it;
    double diff_time;

    gettimeofday(&timer,NULL);

    for(it = cycleVector.begin(); it != cycleVector.end(); it++){
        if (it->first == func){
            double end_time = timer.tv_sec +  timer.tv_usec*1.0/1000000;
            diff_time = end_time - it->second[0];
            //assert(diff_time != 0);
            it->second[2]++;
            it->second[1] += diff_time;
            break;
        }
    }
        
    if(func != "Others" and isOthers == false){
        isOthers = true;
        setInitCycle("Others");
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
    
    stringst << "Dist_" << distFunc << "_" << w << "x" << h;
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