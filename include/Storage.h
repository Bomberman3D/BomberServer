/*                                         *
*            Bomberman Server              *
*                                          *
*    Developed by: Cmaranec (Kennny)       *
*                                          *
*    Copyright (c) 2011                    *
*                                          */

#ifndef __STORAGE_H_
#define __STORAGE_H_

#include "Singleton.h"

class GlobalStorage
{
    public:
        GlobalStorage() {};
        ~GlobalStorage() {};

        void LoadServerInfo();

        uint32 GetId() { return m_id; };
        const char* GetServerName() { return m_serverName.c_str(); };
        const char* GetIP() { return m_ip.c_str(); };
        uint32 GetLock() { return m_lock; };

    protected:
        uint32 m_id;
        std::string m_serverName;
        std::string m_ip;
        uint32 m_lock;
};

static GlobalStorage* sGlobalStorage = Singleton<GlobalStorage>::Instance();

#endif
