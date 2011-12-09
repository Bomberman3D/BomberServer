#include <global.h>
#include "Instance.h"
#include "Map.h"

#include <sstream>

InstanceManager::InstanceManager()
{
}

InstanceManager::~InstanceManager()
{
}

bool InstanceManager::Initialize()
{
    m_Instances.clear();

    return true;
}

bool InstanceManager::InitDefaultInstance()
{
    if (m_Instances.find(0) == m_Instances.end())
    {
        Instance* pNew = new Instance;
        if (!pNew)
            return false;

        pNew->id = 0;
        pNew->instanceName = "Default Instance";
        pNew->mapId = 1;
        pNew->maxplayers = 4;
        pNew->players = 0;

        pNew->GenerateRandomDynamicRecords();

        m_Instances[0] = pNew;

        return true;
    }

    return false;
}

int32 InstanceManager::InitNewInstance()
{
    char instname[32];

    for (uint32 i = 0; i < MAX_INSTANCES; i++)
    {
        if (m_Instances.find(i) == m_Instances.end())
        {
            Instance* pNew = new Instance;
            if (!pNew)
                return -1;

            sprintf(instname, "Instance %i", i);

            pNew->id = i;
            pNew->instanceName = instname;
            pNew->mapId = 1;
            pNew->maxplayers = 4;
            pNew->players = 0;

            pNew->GenerateRandomDynamicRecords();

            m_Instances[i] = pNew;

            return i;
        }
    }

    return -1;
}

const char* InstanceManager::GetInstanceString()
{
    bool first = true;
    std::stringstream str;

    for (std::map<uint32, Instance*>::const_iterator itr = m_Instances.begin(); itr != m_Instances.end(); ++itr)
    {
        if (!first)
             str << "|";
        else
            first = true;

        str << itr->first << "|";
        str << itr->second->instanceName.c_str() << "|";
        str << itr->second->players << "/" << itr->second->maxplayers << "|";
        str << sMapManager->GetMapName(itr->second->mapId);
    }

    str << '\0';

    const std::string retstr(str.str());

    char* pRet = new char[retstr.size()+1];
    strcpy(pRet, retstr.c_str());
    pRet[retstr.size()] = '\0';

    return pRet;
}

void InstanceManager::RegisterPlayer(Player *pPlayer, uint32 instanceId)
{
    Instance* pInstance = m_Instances[instanceId];
    if (!pInstance)
        return;

    if (pInstance->players >= pInstance->maxplayers)
        return;

    for (int i = 0; i < MAX_PLAYERS_PER_INSTANCE; i++)
    {
        if (!pInstance->pPlayers[i])
        {
            pInstance->pPlayers[i] = pPlayer;
            pInstance->players += 1;
            pPlayer->m_modelIdOffset = i;
            return;
        }
    }
}

Instance* InstanceManager::GetPlayerInstance(Player *pPlayer)
{
    if (!pPlayer)
        return NULL;

    for (std::map<uint32, Instance*>::iterator itr = m_Instances.begin(); itr != m_Instances.end(); ++itr)
    {
        for (int i = 0; i < MAX_PLAYERS_PER_INSTANCE; i++)
        {
            if (itr->second->pPlayers[i] == pPlayer)
                return itr->second;
        }
    }

    return NULL;
}

int32 InstanceManager::GetPlayerInstanceId(Player *pPlayer)
{
    if (Instance* pInstance = GetPlayerInstance(pPlayer))
        return pInstance->id;
    else
        return -1;
}

void InstanceManager::RemovePlayerFromInstances(Player* pPlayer)
{
    if (m_Instances.empty())
        return;

    for (std::map<uint32, Instance*>::iterator itr = m_Instances.begin(); itr != m_Instances.end(); ++itr)
    {
        if (!itr->second)
            continue;

        for (uint8 i = 0; i < MAX_PLAYERS_PER_INSTANCE; i++)
            if (itr->second->pPlayers[i] == pPlayer)
            {
                itr->second->pPlayers[i] = NULL;
                if (itr->second->players > 0)
                    itr->second->players -= 1;
            }
    }
}

void InstanceManager::SendInstancePacket(SmartPacket *data, uint32 instanceId)
{
    Instance* pInstance = m_Instances[instanceId];
    if (!pInstance)
        return;

    for (int i = 0; i < MAX_PLAYERS_PER_INSTANCE; i++)
    {
        if (pInstance->pPlayers[i])
            sSession->SendPacket(pInstance->pPlayers[i], data);
    }
}

void Instance::GenerateRandomDynamicRecords()
{
    Map* myMap = sMapManager->GetMap(mapId);
    if (!myMap)
        return;

    m_dynRecords.clear();

    std::list<std::pair<uint32, uint32>> freeSpots;
    freeSpots.clear();

    uint32 width = myMap->field.size();
    uint32 height = myMap->field[0].size();

    for (uint32 i = 0; i < width; i++)
    {
        for (uint32 j = 0; j < height; j++)
        {
            if (myMap->field[i][j].type == 0) // type ground
            {
                if (   !myMap->IsStartLoc(i-1,j-1)
                    && !myMap->IsStartLoc(i-1,j  )
                    && !myMap->IsStartLoc(i-1,j+1)
                    && !myMap->IsStartLoc(i+1,j-1)
                    && !myMap->IsStartLoc(i+1,j  )
                    && !myMap->IsStartLoc(i+1,j+1)
                    && !myMap->IsStartLoc(i  ,j-1)
                    && !myMap->IsStartLoc(i  ,j+1))
                    freeSpots.push_back(std::make_pair<uint32, uint32>(i,j));
            }
        }
    }

    DynamicRecord temp;
    for(std::list<std::pair<uint32, uint32>>::const_iterator itr = freeSpots.begin(); itr != freeSpots.end(); ++itr)
    {
        if (rand()%2 > 0)
        {
            temp.x = (*itr).first;
            temp.y = (*itr).second;
            temp.type = 1;
            m_dynRecords.push_back(temp);
        }
    }
}
