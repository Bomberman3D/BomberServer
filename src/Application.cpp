/*                                         *
*            Bomberman Server              *
*                                          *
*    Developed by: Cmaranec (Kennny)       *
*                                          *
*    Copyright (c) 2011                    *
*                                          */

#include <global.h>
#include "Application.h"
#include "Singleton.h"
#include "Log.h"
#include "Config.h"
#include "Storage.h"
#include "Session.h"
#include "Map.h"
#include "Instance.h"

#include <ctime>

Application::Application()
{
    m_commandQueueLock = false;

    for (uint32 i = 0; i < MAX_DEBUG_SUBJECT; i++)
        m_debugState[i] = false;
}

void Application::init()
{
    sLog->init();
    sLog->StaticOut("---------------------------------");
    sLog->StaticOut("-      Bomberman Server         -");
    sLog->StaticOut("- For Bomberman3D project       -");
    sLog->StaticOut("-                               -");
    sLog->StaticOut("- Created by Cmaranec (Kennny)  -");
    sLog->StaticOut("- Copyright (c) 2011            -");
    sLog->StaticOut("---------------------------------");
    sLog->StaticOut("");

    srand ( (unsigned int)time(NULL) );

    sLog->StaticOut(">> Loading config...");
    if (!sConfig->LoadConfig("main.conf"))
        abort("Loading config main.conf failed");
    sLog->StaticOut("Config loaded");

    sLog->StaticOut("");

    sLog->StaticOut(">> Loading server info...");
    sGlobalStorage->LoadServerInfo();
    sLog->StaticOut("Server info loaded");

    sLog->StaticOut("");

    sLog->StaticOut(">> Starting network services...");
    sSession->Startup();
    sLog->StaticOut("Network services started");

    sLog->StaticOut("");

    sLog->StaticOut(">> Loading maps...");
    if (!sMapManager->Initialize())
        abort("Could not load maps");
    sLog->StaticOut("Maps loaded");

    sLog->StaticOut("");

    sLog->StaticOut(">> Initializing instance system");
    if (!sInstanceManager->Initialize())
        abort("Could not initialize instance manager");
    if (!sInstanceManager->InitDefaultInstance())
        abort("Could not initialize default instance");
    sLog->StaticOut("Instance manager initialized");
    sLog->StaticOut("Running default instance 0");

    sLog->StaticOut("");
}

void Application::Update()
{
    while (1)
    {
        while (m_commandQueueLock)
            boost::this_thread::yield();

        m_commandQueueLock = true;
        for (std::list< std::string >::iterator itr = m_commandQueue.begin(); itr != m_commandQueue.end(); )
        {
            if (!HandleConsoleCommand((*itr).c_str()))
                sLog->StringOut("No such command %s", (*itr).c_str());

            itr = m_commandQueue.erase(itr);
        }
        m_commandQueueLock = false;

        sInstanceManager->Update();

        boost::this_thread::yield();
    }
}

void runAppUpdateThread()
{
    sApp->Update();
}

void Application::run()
{
    sLog->StaticOut("Successfully loaded!");
    sLog->StringOut("Running ID %u: %s",sGlobalStorage->GetId(),sGlobalStorage->GetServerName());
    sLog->StaticOut("");

    sLog->StaticOut(">> Starting threads..");
    sLog->StaticOut("Starting network worker thread...");
    sLog->StaticOut("Starting network acceptor thread...");
    boost::thread networkWorker(runSessionWorker);

    uint64 sw = clock() / CLOCK_MOD;
    bool ready = false;
    for (uint64 wait = sw; wait <= sw+5000; wait = clock() / CLOCK_MOD)
    {
        ready = ThreadStatus.sessionworker;

        if (ready)
            break;
    }
    if (!ready)
        abort("Failed to start all threads in 5 seconds time limit");

    sLog->StaticOut("");
    sLog->StaticOut("All support threads running");
    sLog->StaticOut("Starting update thread...");

    boost::thread updateThread(runAppUpdateThread);

    sLog->StaticOut("Update thread running!");

    sLog->StaticOut("");

    sLog->StaticOut("Ready!\n");

    std::string input; // Used for storing user input

    while(1)
    {
        input.clear();
        std::getline(std::cin,input);

        while (m_commandQueueLock)
            boost::this_thread::yield();

        m_commandQueueLock = true;
        m_commandQueue.push_back(input.c_str());
        m_commandQueueLock = false;

        boost::this_thread::yield();
    }
}

void Application::abort(const char* reason)
{
    sLog->ErrorOut("An exception has occured while running application: %s",reason);
    sLog->StaticOut("Press any key to terminate application...");
    std::cin.get();
    exit(-1);
}

void Application::abort()
{
    exit(-1);
}

bool Application::HandleConsoleCommand(const char *input)
{
    int cmpres;   // Used for storing string compare result

    if ((cmpres = mstrcmp("broadcast",input)) == 1)
    {
        char* arg = new char[1024];
        strcpy(arg,&(input[10]));
        SmartPacket data(SMSG_SERVER_BROADCAST);
        data << arg;
        sSession->BroadcastPacket(&data);
        sLog->StringOut("Broadcasting of message '%s' was successful!",arg);
        return true;
    }
    else if (cmpres == 0)
    {
        sLog->StringOut("Usage: broadcast $message\n$message - message to be broadcast");
        return true;
    }
    else if ((cmpres = mstrcmp("exit",input)) == 0)
    {
        sLog->StringOut("Exit requested from console command. Exiting...");
        exit(0);
        return true;
    }
    else if (cmpres == 1)
    {
        sLog->StringOut("Usage: exit\nNo arguments");
        return true;
    }
    else if ((cmpres = mstrcmp("debug",input)) == 1 || cmpres == 0)
    {
        char* arg = new char[1024];
        strcpy(arg,&(input[6]));

        if (cmpres == 1)
        {
            if (mstrcmp("network", arg) == 0)
            {
                if (sApp->isDebug(DEBUG_NETWORK))
                {
                    sLog->StringOut("Debugging mode for networking is now shut down");
                    sApp->setDebug(DEBUG_NETWORK, false);
                }
                else
                {
                    sLog->StringOut("Starting debugging mode for networking");
                    sApp->setDebug(DEBUG_NETWORK, true);
                }
            }
            else if (mstrcmp("system", arg) == 0)
            {
                if (sApp->isDebug(DEBUG_SYSTEM))
                {
                    sLog->StringOut("Debugging mode for system stuff is now shut down");
                    sApp->setDebug(DEBUG_SYSTEM, false);
                }
                else
                {
                    sLog->StringOut("Starting debugging mode for system stuff");
                    sApp->setDebug(DEBUG_SYSTEM, true);
                }
            }
            else
                cmpres = 0;
        }

        if (cmpres == 0)
        {
            sLog->StringOut("Usage: debug $subject\n$subject - subject to debug");
            sLog->StringOut("\nAvailable subjects:");

            sLog->StringOut("network  - debugs network traffic");
            sLog->StringOut("system   - debugs other system stuff");
        }
        return true;
    }

    return false;
}
