/*                                         *
*            Bomberman Server              *
*                                          *
*    Developed by: Cmaranec (Kennny)       *
*                                          *
*    Copyright (c) 2011                    *
*                                          */

#include <global.h>
#include "Session.h"
#include "Log.h"
#include "Application.h"
#include "Config.h"
#include "Instance.h"

#include <boost/thread.hpp>
#include <boost/date_time.hpp>

bool Session::Startup()
{
    m_port = sConfig->GetIntValue(CONFIG_MAIN_PORT);

#ifdef _WIN32
    WORD version = MAKEWORD(1,1);
    WSADATA data;
    if (WSAStartup(version, &data) != 0)
    {
        sLog->ErrorOut("Failed to start network service");
        return false;
    }
#endif

    if ((m_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1)
    {
        sLog->ErrorOut("Failed to create socket");
        return false;
    }

    m_sockName.sin_family = AF_INET;
    m_sockName.sin_port = htons(m_port);
    m_sockName.sin_addr.s_addr = INADDR_ANY;
    if (bind(m_socket, (sockaddr*)&m_sockName, sizeof(m_sockName)) == -1)
    {
        sLog->ErrorOut("Failed to bind socket");
        return false;
    }

    if (listen(m_socket, 10) == -1)
    {
        sLog->ErrorOut("Couldn't create connection queue");
        return false;
    }

#ifdef _WIN32
    u_long arg = 1;
    if (ioctlsocket(m_socket, FIONBIO, &arg) == SOCKET_ERROR)
    {
        sLog->ErrorOut("Failed to switch socket to non-blocking mode");
    }
#else
    int oldFlag = fcntl(m_socket, F_GETFL, 0);
    if (fcntl(m_socket, F_SETFL, oldFlag | O_NONBLOCK) == -1)
    {
        sLog->ErrorOut("Failed to switch socket to non-blocking mode");
    }
#endif

    return true;
}

void Session::Worker()
{
    sLog->StaticOut("Network worker thread running");
    sApp->ThreadStatus.sessionworker = true;

    char* buf = new char[BUFFER_LEN];
    int result, error;
    SOCK res;
    Player* pClient = NULL;
    Player* pNew = new Player;
    pNew->m_addrLen = sizeof(pNew->m_sockInfo);
    std::list<Player*>::iterator itr;

    while(1)
    {
        res = accept(m_socket, (sockaddr*)&pNew->m_sockInfo, &pNew->m_addrLen);
        error = LASTERROR();

        if (res == INVALID_SOCKET && error == SOCKETWOULDBLOCK)
        {
            // zadne prichozi spojeni
        }
        else if (res == INVALID_SOCKET && error != SOCKETWOULDBLOCK)
        {
            sLog->ErrorOut("Socket error: %i", error);
        }
        else
        {
            pNew->m_socket = res;
            pNew->m_addrLen = sizeof(pNew->m_sockInfo);

            sLog->DebugOut("Accepting connection from %s",inet_ntoa((in_addr)pNew->m_sockInfo.sin_addr));

            clientList.push_back(pNew);
            sLog->NetworkDebugOut(pNew,"Inserted into client list");

            pNew = new Player;
            pNew->m_addrLen = sizeof(pNew->m_sockInfo);
        }

        if (clientList.empty())
        {
            // Wait for longer period, no need to check for socket traffic so often if nobody's connected
            boost::this_thread::sleep(boost::posix_time::milliseconds(100));
            continue;
        }

        for (itr = clientList.begin(); itr != clientList.end();)
        {
            pClient = (*itr); // Who wants ugly star in brackets?!
            if (pClient)
            {
                memset(buf,0,BUFFER_LEN);
                result = recv(pClient->m_socket, buf, BUFFER_LEN, 0);
                error = LASTERROR();

                if (result > 0)
                {
                    sLog->NetworkDebugOut(pClient,"Received data, size: %u",result);
                    SmartPacket* parsed = BuildPacket(buf,result);
                    if (parsed)
                    {
                        sLog->NetworkDebugOut(pClient,"Opcode 0x%.2X, size %u", parsed->GetOpcode(), parsed->GetSize());
                        ProcessPacket(parsed, pClient);

                        int32 totalparsed = parsed->GetSize()+8;
                        while (totalparsed < result)
                        {
                            parsed = BuildPacket(buf+totalparsed, result-totalparsed);
                            if (parsed)
                            {
                                sLog->NetworkDebugOut(pClient,"Parsed additional %u bytes",parsed->GetSize());
                                sLog->NetworkDebugOut(pClient,"Opcode 0x%.2X, size %u", parsed->GetOpcode(), parsed->GetSize());
                                ProcessPacket(parsed, pClient);
                                totalparsed += parsed->GetSize()+8;
                            }
                            else
                                break;
                        }
                    }
                }
                else if (result == 0 || error == SOCKETCONNRESET)
                {
                    sLog->NetworkDebugOut(pClient,"Client disconnected");
                    sInstanceManager->RemovePlayerFromInstances(pClient);
                    itr = clientList.erase(itr);
                    continue;
                }
                else
                {
                    if (error != SOCKETWOULDBLOCK && error != 0)
                    {
                        //error
                        sLog->NetworkOut(pClient,"Unhandled socket error: %u",error);
                    }
                }
            }
            ++itr;
        }

        // Some waiting time, could be changed if needed, but thread have to wait since select() is not present
        // technically, this counts as "yield" on Windows
        boost::this_thread::sleep(boost::posix_time::milliseconds(1));
    }
}

void runSessionWorker()
{
    sSession->Worker();
}

Player* Session::GetPlayerByName(const char* name)
{
    if (!name)
        return NULL;

    for (std::list<Player*>::iterator itr = clientList.begin(); itr != clientList.end(); ++itr)
    {
        if (mstrcmp((*itr)->m_nickName.c_str(), name) == 0)
            return (*itr);
    }

    return NULL;
}

Player* Session::GetPlayerById(uint32 id)
{
    if (id == 0)
        return NULL;

    for (std::list<Player*>::iterator itr = clientList.begin(); itr != clientList.end(); ++itr)
    {
        if (id == (*itr)->m_socket)
            return (*itr);
    }

    return NULL;
}
