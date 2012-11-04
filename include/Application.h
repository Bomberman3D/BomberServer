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

class Application
{
  public:
    Application() { m_commandQueueLock = false; }

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
            sessionacceptor = false;
        }
        bool sessionworker;
        bool sessionacceptor;
    } ThreadStatus;

  private:
      std::list< std::string > m_commandQueue;
      bool m_commandQueueLock;
};

static Application* sApp = Singleton<Application>::Instance();

#endif
