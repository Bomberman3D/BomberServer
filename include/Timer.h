/*                                         *
*            Bomberman Server              *
*                                          *
*    Developed by: Cmaranec (Kennny)       *
*                                          *
*    Copyright (c) 2011                    *
*                                          */

#ifndef __TIMER_H_
#define __TIMER_H_

#include <global.h>
#include "Singleton.h"

#include <list>

struct TimerRecord
{
    uint32 maxTime;
    uint32 actualTime;
    uint64 param1;
    uint64 param2;
    uint64 param3;
    uint64 param4;
    void (*Handler)(uint64, uint64, uint64, uint64);
};

class Timer
{
    public:
        Timer();
        ~Timer();

        void Worker();

        void AddTimedEvent(uint32 maxTime, void (*Handler)(uint64,uint64,uint64,uint64), uint64 p1 = 0, uint64 p2 = 0, uint64 p3 = 0, uint64 p4 = 0);

    protected:
        std::list<TimerRecord> timedEvents;
};

static Timer* sTimer = Singleton<Timer>::Instance();

extern void runTimerWorker();

#endif
