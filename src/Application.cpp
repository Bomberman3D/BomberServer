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
#include "Timer.h"
#include "Session.h"
#include "Map.h"
#include "Instance.h"

#include <ctime>
#include <boost/thread.hpp>

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

void Application::run()
{
    sLog->StaticOut("Successfully loaded!");
    sLog->StringOut("Running ID %u: %s",sGlobalStorage->GetId(),sGlobalStorage->GetServerName());
    sLog->StaticOut("");

    sLog->StaticOut(">> Starting threads..");
    sLog->StaticOut("Starting network worker thread...");
    sLog->StaticOut("Starting network acceptor thread...");
    boost::thread networkWorker(runSessionWorker);
    boost::thread networkAcceptor(runSessionAcceptor);

    uint64 sw = clock() / CLOCK_MOD;
    bool ready = false;
    for (uint64 wait = sw; wait <= sw+5000; wait = clock() / CLOCK_MOD)
    {
        ready = ThreadStatus.sessionworker
            && ThreadStatus.sessionacceptor;

        if (ready)
            break;
    }
    if (!ready)
        abort("Failed to start all threads in 5 seconds time limit");

    sLog->StaticOut("");
    sLog->StaticOut("All threads running");

    std::string input; // Used for storing user input

    while(1)
    {
        input.clear();
        std::getline(std::cin,input);

        if (!HandleConsoleCommand(input.c_str()))
            sLog->StringOut("No such command.");

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
    return false;
}
