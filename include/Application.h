/*                                         *
*            Bomberman Server              *
*                                          *
*    Developed by: Cmaranec (Kennny)       *
*                                          *
*    Copyright (c) 2011                    *
*                                          */

#ifndef __APPLICATION_H_
#define __APPLICATION_H_

#include "global.h"
#include "Singleton.h"

enum DebugSubjects
{
    DEBUG_SYSTEM      = 0,
    DEBUG_NETWORK     = 1,
    MAX_DEBUG_SUBJECT
};

class Application
{
  public:
    Application();

    void init();
    void abort(const char* reason);
    void abort();
    void run();

    void Update();

    bool HandleConsoleCommand(const char* input);

    struct thr_status
    {
        thr_status()
        {
            sessionworker = false;
        }
        bool sessionworker;
    } ThreadStatus;

    bool isDebug(uint8 subject)
    {
        if (subject < MAX_DEBUG_SUBJECT)
            return m_debugState[subject];
        else
            return false;
    };
    void setDebug(uint8 subject, bool state = true)
    {
        if (subject < MAX_DEBUG_SUBJECT)
            m_debugState[subject] = state;
    }

  private:
      std::list< std::string > m_commandQueue;
      bool m_commandQueueLock;
      bool m_debugState[MAX_DEBUG_SUBJECT];
};

static Application* sApp = Singleton<Application>::Instance();

#endif
