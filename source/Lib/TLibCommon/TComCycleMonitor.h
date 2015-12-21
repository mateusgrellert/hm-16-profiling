/* 
 * File:   TComCycleMonitor.h
 * Author: grellert
 *
 * Created on April 16, 2014, 1:21 PM
 */

#ifndef TCOMCYCLEMONITOR_H
#define	TCOMCYCLEMONITOR_H

#define USE_RDTSC 1
#define CPU_TICKS_PER_SECOND 3400000000.0
#define EN_CYCLE_MONITOR 1
#define FUNCTION_LEVEL_MONITORING 1

#define BETWEEN(x,a,b) ((x) >= (a) && (x) <= (b)) ? true : false

#include <cstdlib>
#include <cstdio>
#include <vector>
#include <string>

typedef unsigned int uint;

using namespace std;

typedef pair<string, vector<double> > dictionary; // (func, cycle_ini, cycle_average, ncalls)

class TComCycleMonitor {
public:
    static vector<dictionary> cycleVector;
    static ofstream cycleResults;
   // static uint currFrame;
    static struct timespec begints;

    static struct timeval timer;
    static struct timespec ts_start;
    static struct timespec ts_end;
    
    static std::string currDistFunc;
    static uint currDepth;
    static bool isIntra;
    static bool isMerge;
    static bool isOthers;
    
    TComCycleMonitor();
    static void init(double qp);
    static dictionary createTriplet(string);
    static void setCurrDepth(uint d);

    static void setInitCycle(string, uint d);
    static void setEndCycle(string);
    static void reportCycles(int);
    static void setDistFuncString(int w, int h, int func_enum);
    static void InitRdtsc();
    static struct timespec* TimeSpecDiff(struct timespec *ts1, struct timespec *ts2);

};

#if defined(__i386__)

static __inline__ unsigned long long rdtsc(void)
{
  unsigned long long int x;
     __asm__ volatile (".byte 0x0f, 0x31" : "=A" (x));
     return x;
}
#elif defined(__x86_64__)


static __inline__ unsigned long long rdtsc(void)
{
  unsigned hi, lo;
  __asm__ __volatile__ ("rdtsc" : "=a"(lo), "=d"(hi));
  return ( (unsigned long long)lo)|( ((unsigned long long)hi)<<32 );
}

#elif defined(__powerpc__)


static __inline__ unsigned long long rdtsc(void)
{
  unsigned long long int result=0;
  unsigned long int upper, lower,tmp;
  __asm__ volatile(
                "0:                  \n"
                "\tmftbu   %0           \n"
                "\tmftb    %1           \n"
                "\tmftbu   %2           \n"
                "\tcmpw    %2,%0        \n"
                "\tbne     0b         \n"
                : "=r"(upper),"=r"(lower),"=r"(tmp)
                );
  result = upper;
  result = result<<32;
  result = result|lower;

  return(result);
}

#endif

#endif	/* TCOMCYCLEMONITOR_H */

