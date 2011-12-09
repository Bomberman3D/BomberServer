/*                                         *
*            Bomberman Server              *
*                                          *
*    Developed by: Cmaranec (Kennny)       *
*                                          *
*    Copyright (c) 2011                    *
*                                          */

#ifndef __SESSION_H_
#define __SESSION_H_

#include "Singleton.h"
#include "Packets/SmartPacket.h"

#include <list>

#ifdef _WIN32
 #include <windows.h>
 #include <winsock.h>

 #define SOCK SOCKET
 #define ADDRLEN int

 #define SOCKETWOULDBLOCK WSAEWOULDBLOCK
 #define SOCKETCONNRESET  WSAECONNRESET
 #define LASTERROR() WSAGetLastError()
#else
 #include <iostream>
 #include <sys/types.h>
 #include <sys/socket.h>
 #include <netinet/in.h>
 #include <arpa/inet.h>
 #include <unistd.h>
 #include <string>
 #include <netdb.h>
 #include <fcntl.h>

 #define SOCK int
 #define ADDRLEN socklen_t

 #define INVALID_SOCKET -1

 #define SOCKETWOULDBLOCK EAGAIN
// Is this right? TODO: find proper value for linux
 #define SOCKETCONNRESET  10054L
 #define LASTERROR() errno
#endif

// Network buffer length = 1 kB
// If necessary, you can change it
#define BUFFER_LEN 1*1024

struct Client
{
    SOCK m_socket;
    hostent* m_host;
    sockaddr_in m_sockInfo;
    ADDRLEN m_addrLen;
};

#include "Player.h"
struct Player;

class Session
{
    public:
        Session() {};
        ~Session() {};

        bool Startup();

        void Worker();
        void Acceptor();

        SmartPacket* BuildPacket(const char* buffer, uint32 size);
        void BroadcastPacket(SmartPacket* data);
        void SendPacket(Client* pClient, SmartPacket* data);
        void SendPacket(SOCK socket, SmartPacket* data);

        void ProcessPacket(SmartPacket* packet, Player* pSource);

        Player* GetPlayerByName(const char* name);

    protected:
        std::list<Player*> clientList;

        SOCK m_socket;
        int m_port;
        sockaddr_in m_sockName;
};

static Session* sSession = Singleton<Session>::Instance();

extern void runSessionWorker();
extern void runSessionAcceptor();

#endif
