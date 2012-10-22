/*                                         *
*            Bomberman Server              *
*                                          *
*    Developed by: Cmaranec (Kennny)       *
*                                          *
*    Copyright (c) 2011                    *
*                                          */

#include <global.h>
#include "Timer.h"
#include "Log.h"
#include "Application.h"

#include <ctime>
#include <boost/thread.hpp>
#include <boost/date_time.hpp>

Timer::Timer()
{
}

Timer::~Timer()
{
}

void Timer::Worker()
{
    sLog->StaticOut("Timer thread running");
    sApp->ThreadStatus.timer = true;
    uint64 last = (uint64)(clock() / CLOCK_MOD);
    uint32 diff;
    std::list<TimerRecord>::iterator itr;

    while(1)
    {
        diff = (uint32)((uint64)(clock() / CLOCK_MOD)-last);
        if (!timedEvents.empty())
        {
            for (itr = timedEvents.begin(); itr != timedEvents.end();)
            {
                if ((*itr).Handler)
                {
                    diff = (uint32)((uint64)(clock() / CLOCK_MOD)-last);
                    if ((*itr).actualTime <= diff)
                    {
                        (*itr).Handler((*itr).param1,(*itr).param2,(*itr).param3,(*itr).param4);
                        itr = timedEvents.erase(itr);
                        continue;
                    } else (*itr).actualTime -= diff;
                }
                else
                {
                    itr = timedEvents.erase(itr);
                    continue;
                }
                ++itr;
            }
        }

        last = (uint64)(clock() / CLOCK_MOD);

        // Some waiting time, could be increased if needed
        boost::this_thread::yield();
    }
}

void runTimerWorker()
{
    sTimer->Worker();
}

void Timer::AddTimedEvent(uint32 maxTime, void (*Handler)(uint64,uint64,uint64,uint64), uint64 p1, uint64 p2, uint64 p3, uint64 p4)
{
    TimerRecord pTemp;

    pTemp.maxTime = maxTime;
    pTemp.actualTime = maxTime;
    pTemp.Handler = Handler;
    pTemp.param1 = p1;
    pTemp.param2 = p2;
    pTemp.param3 = p3;
    pTemp.param4 = p4;

    timedEvents.push_back(pTemp);
}
