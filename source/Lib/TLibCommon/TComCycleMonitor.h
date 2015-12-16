/* 
 * File:   TComCycleMonitor.h
 * Author: grellert
 *
 * Created on April 16, 2014, 1:21 PM
 */

#ifndef TCOMCYCLEMONITOR_H
#define	TCOMCYCLEMONITOR_H


#define EN_CYCLE_MONITOR 1
#define FUNCTION_LEVEL_MONITORING 1

#define BETWEEN(x,a,b) ((x) >= (a) && (x) <= (b)) ? true : false

#include <cstdlib>
#include <cstdio>
#include <vector>
#include <string>


using namespace std;

typedef pair<string, vector<double> > dictionary; // (func, cycle_ini, cycle_average, ncalls)

class TComCycleMonitor {
public:
    static vector<dictionary> cycleVector;
    static ofstream cycleResults;
    static uint currFrame;
    static struct timespec begints;

    static struct timeval timer;
    static std::string currDistFunc;
    static bool isIntra;
    static bool isMerge;
    static bool isOthers;
    
    TComCycleMonitor();
    static void init();
    static dictionary createTriplet(string);
    static void setInitCycle(string);
    static void setEndCycle(string);
    static void reportCycles(int);
    static void setDistFuncString(int w, int h, int func_enum);
    static void InitRdtsc();
    static struct timespec* TimeSpecDiff(struct timespec *ts1, struct timespec *ts2);

};

#endif	/* TCOMCYCLEMONITOR_H */

