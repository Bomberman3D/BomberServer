/*                                         *
*            Bomberman Server              *
*                                          *
*    Developed by: Cmaranec (Kennny)       *
*                                          *
*    Copyright (c) 2011                    *
*                                          */

#ifndef __LOG_H_
#define __LOG_H_

#include "Singleton.h"
#include "Session.h"

class Log
{
  public:
    Log() { }

    void init();

    void StaticOut(const char* c);
    void StringOut(const char* str, ...);
    void StringOutLine(const char* str, ...);
    void ErrorOut(const char* str, ...);
    void NetworkOut(Client* pClient, const char* str, ...);

    void SetConsoleColor(int32 color);
};

static Log* sLog = Singleton<Log>::Instance();

#endif
