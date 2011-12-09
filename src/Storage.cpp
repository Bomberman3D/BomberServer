/*                                         *
*            Bomberman Server              *
*                                          *
*    Developed by: Cmaranec (Kennny)       *
*                                          *
*    Copyright (c) 2011                    *
*                                          */

#include <global.h>
#include "Storage.h"

void GlobalStorage::LoadServerInfo()
{
    // TODO: nacteni z configu

    m_id = 1;
    m_serverName = "Bomberman3D Main Server";
    m_ip = "127.0.0.1";
    m_lock = 0;
}
