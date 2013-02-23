#include <global.h>
#include "Instance.h"
#include "Map.h"
#include "Log.h"

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

    while (pInstance->m_playerMapLock)
        boost::this_thread::yield();

    pInstance->m_playerMapLock = true;

    Map* pMap = sMapManager->GetMap(pInstance->mapId);

    for (int i = 0; i < MAX_PLAYERS_PER_INSTANCE; i++)
    {
        if (!pInstance->pPlayers[i])
        {
            pInstance->pPlayers[i] = pPlayer;
            pInstance->players += 1;
            pPlayer->m_modelIdOffset = i;

            if (pMap)
            {
                if (pMap->startloc[i*2] != 0 && pMap->startloc[i*2+1] != 0)
                {
                    pPlayer->m_positionX = pMap->startloc[i*2];
                    pPlayer->m_positionY = pMap->startloc[i*2+1];
                }
            }

            pPlayer->m_respawnTime = 0;

            pPlayer->score.deaths = 0;
            pPlayer->score.kills = 0;

            pInstance->SendScoreBoard();

            pInstance->m_playerMapLock = false;
            return;
        }
    }

    pInstance->m_playerMapLock = false;
}

Instance* InstanceManager::GetPlayerInstance(Player *pPlayer)
{
    if (!pPlayer)
        return NULL;

    for (std::map<uint32, Instance*>::iterator itr = m_Instances.begin(); itr != m_Instances.end(); ++itr)
    {
        /*while (itr->second->m_playerMapLock)
            boost::this_thread::yield();

        itr->second->m_playerMapLock = true;*/

        for (int i = 0; i < MAX_PLAYERS_PER_INSTANCE; i++)
        {
            if (itr->second->pPlayers[i] == pPlayer)
            {
                //itr->second->m_playerMapLock = false;
                return itr->second;
            }
        }

        //itr->second->m_playerMapLock = false;
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

        while (itr->second->m_playerMapLock)
            boost::this_thread::yield();

        itr->second->m_playerMapLock = true;

        for (uint8 i = 0; i < MAX_PLAYERS_PER_INSTANCE; i++)
            if (itr->second->pPlayers[i] == pPlayer)
            {
                itr->second->pPlayers[i] = NULL;
                if (itr->second->players > 0)
                    itr->second->players -= 1;
            }

        itr->second->m_playerMapLock = false;
    }
}

void InstanceManager::SendInstancePacket(SmartPacket *data, uint32 instanceId, bool alreadyLocked)
{
    Instance* pInstance = m_Instances[instanceId];
    if (!pInstance)
        return;

    /*if (!alreadyLocked)
    {
        while (!alreadyLocked && pInstance->m_playerMapLock)
            boost::this_thread::yield();

        pInstance->m_playerMapLock = true;
    }*/

    for (int i = 0; i < MAX_PLAYERS_PER_INSTANCE; i++)
    {
        if (pInstance->pPlayers[i])
            sSession->SendPacket(pInstance->pPlayers[i], data);
    }

    /*if (!alreadyLocked)
        pInstance->m_playerMapLock = false;*/
}

void Instance::GenerateRandomDynamicRecords()
{
    Map* myMap = sMapManager->GetMap(mapId);
    if (!myMap)
        return;

    m_dynRecords.clear();

    std::list< std::pair<uint32, uint32> > freeSpots;
    freeSpots.clear();

    uint32 width = myMap->field.size();
    uint32 height = myMap->field[0].size();

    for (uint32 i = 0; i < width; i++)
    {
        for (uint32 j = 0; j < height; j++)
        {
            if (myMap->field[i][j].type == 0) // type ground
            {
                if (!myMap->IsNearStartLoc(i,j))
                    freeSpots.push_back(std::make_pair<uint32, uint32>(i,j));
            }
        }
    }

    DynamicRecord temp;
    for(std::list< std::pair<uint32, uint32> >::const_iterator itr = freeSpots.begin(); itr != freeSpots.end(); ++itr)
    {
        if (rand()%2 > 0)
        {
            temp.x = (*itr).first;
            temp.y = (*itr).second;
            temp.type = DYNAMIC_TYPE_BOX;
            m_dynRecords.push_back(temp);
        }
    }
}

bool Instance::AddBomb(uint32 x, uint32 y, uint32 owner, uint32 reach)
{
    Map* myMap = sMapManager->GetMap(mapId);
    if (!myMap)
        return false;

    if (myMap->field.size() < x || myMap->field[0].size() < y)
        return false;

    while (m_sharedMapsLock)
        boost::this_thread::yield();

    m_sharedMapsLock = true;

    DynamicRecord tmp;

    tmp.x = x;
    tmp.y = y;
    tmp.type = DYNAMIC_TYPE_BOMB;

    m_dynRecords.push_back(tmp);

    BombRecord bomb;
    bomb.x = x;
    bomb.y = y;
    bomb.owner = owner;
    bomb.boomTime = clock()+2500;
    bomb.reach = reach;

    m_bombMap.push_back(bomb);

    m_sharedMapsLock = false;

    return true;
}

void InstanceManager::Update()
{
    for (std::map<uint32, Instance*>::iterator itr = m_Instances.begin(); itr != m_Instances.end(); ++itr)
        if ((*itr).second)
            (*itr).second->Update();
}

uint32 Instance::GetBombingDistanceInDir(uint32 x, uint32 y, uint32 reach, uint8 dir)
{
    int32 finalreach = -1;
    Map* map = sMapManager->GetMap(mapId);

    if (dir == DIR_X_POS)
    {
        for (int32 i = x; i <= int32(x+reach); i++)
        {
            if (i >= int32(map->field.size()))
            {
                finalreach = map->field.size()-1;
                break;
            }

            if (map->field[i][y].type == TYPE_SOLID_BOX || map->field[i][y].type == TYPE_ROCK)
            {
                finalreach = i-1;
                break;
            }

            for (std::list<DynamicRecord>::iterator itr = m_dynRecords.begin(); itr != m_dynRecords.end(); ++itr)
            {
                if ((*itr).x == i && (*itr).y == y &&
                    ((*itr).type == DYNAMIC_TYPE_BOX/* || (*itr).type == DYNAMIC_TYPE_BOMB*/))
                {
                    finalreach = i;
                    break;
                }
            }
            if (finalreach != -1)
                break;
        }
    }
    else if (dir == DIR_X_NEG)
    {
        for (int32 i = x; i >= int32(x-reach); i--)
        {
            if (i <= 0)
            {
                finalreach = i+1;
                break;
            }

            if (map->field[i][y].type == TYPE_SOLID_BOX || map->field[i][y].type == TYPE_ROCK)
            {
                finalreach = i+1;
                break;
            }

            for (std::list<DynamicRecord>::iterator itr = m_dynRecords.begin(); itr != m_dynRecords.end(); ++itr)
            {
                if ((*itr).x == i && (*itr).y == y &&
                    ((*itr).type == DYNAMIC_TYPE_BOX/* || (*itr).type == DYNAMIC_TYPE_BOMB*/))
                {
                    finalreach = i;
                    break;
                }
            }
            if (finalreach != -1)
                break;
        }
    }
    else if (dir == DIR_Y_POS)
    {
        for (int32 i = y; i <= int32(y+reach); i++)
        {
            if (i >= int32(map->field[0].size()))
            {
                finalreach = map->field[0].size()-1;
                break;
            }

            if (map->field[x][i].type == TYPE_SOLID_BOX || map->field[x][i].type == TYPE_ROCK)
            {
                finalreach = i-1;
                break;
            }

            for (std::list<DynamicRecord>::iterator itr = m_dynRecords.begin(); itr != m_dynRecords.end(); ++itr)
            {
                if ((*itr).x == x && (*itr).y == i &&
                    ((*itr).type == DYNAMIC_TYPE_BOX/* || (*itr).type == DYNAMIC_TYPE_BOMB*/))
                {
                    finalreach = i;
                    break;
                }
            }
            if (finalreach != -1)
                break;
        }
    }
    else if (dir == DIR_Y_NEG)
    {
        for (int32 i = y; i >= int32(y-reach); i--)
        {
            if (i <= 0)
            {
                finalreach = i+1;
                break;
            }

            if (map->field[x][i].type == TYPE_SOLID_BOX || map->field[x][i].type == TYPE_ROCK)
            {
                finalreach = i+1;
                break;
            }

            for (std::list<DynamicRecord>::iterator itr = m_dynRecords.begin(); itr != m_dynRecords.end(); ++itr)
            {
                if ((*itr).x == x && (*itr).y == i &&
                    ((*itr).type == DYNAMIC_TYPE_BOX/* || (*itr).type == DYNAMIC_TYPE_BOMB*/))
                {
                    finalreach = i;
                    break;
                }
            }
            if (finalreach != -1)
                break;
        }
    }

    if (finalreach == -1)
        return reach;

    if (dir == DIR_X_POS || dir == DIR_X_NEG)
        return abs(int32(x) - finalreach);
    else
        return abs(int32(y) - finalreach);
}

void Instance::Update()
{
    uint32 i;
    uint32 bombDists[4];
    DangerousField df;

    for (std::list<BombRecord>::iterator itr = m_bombMap.begin(); itr != m_bombMap.end(); )
    {
        // boom!
        if ((*itr).boomTime < clock())
        {
            for (i = 0; i < 4; i++)
                bombDists[i] = GetBombingDistanceInDir((*itr).x, (*itr).y, (*itr).reach, i);

            // ! Debug vypis mapy !
            /*Map* map = sMapManager->GetMap(mapId);
            bool found = false;
            for (uint32 j = 0; j < map->field.size(); j++)
            {
                for (i = 0; i < map->field[j].size(); i++)
                {
                    found = false;
                    for (std::list<DynamicRecord>::iterator ittr = m_dynRecords.begin(); ittr != m_dynRecords.end(); ++ittr)
                        if (ittr->x == i && ittr->y == j)
                        {
                            sLog->SetConsoleColor(1);
                            sLog->StringOutLine("%u", ittr->type);
                            sLog->SetConsoleColor(0);
                            found = true;
                            break;
                        }
                    if (!found)
                    {
                        if (i <= (*itr).x + bombDists[DIR_X_POS] && i >= (*itr).x && j == (*itr).y)
                            sLog->SetConsoleColor(3);
                        if (i >= (*itr).x - bombDists[DIR_X_NEG] && i <= (*itr).x && j == (*itr).y)
                            sLog->SetConsoleColor(3);
                        if (j <= (*itr).y + bombDists[DIR_Y_POS] && j >= (*itr).y && i == (*itr).x)
                            sLog->SetConsoleColor(3);
                        if (j >= (*itr).y + bombDists[DIR_Y_NEG] && j <= (*itr).y && i == (*itr).x)
                            sLog->SetConsoleColor(3);

                        sLog->StringOutLine("%u", map->field[i][j].type);

                        sLog->SetConsoleColor(0);
                    }
                }
                sLog->StaticOut("");
            }*/

            Player* owner = sSession->GetPlayerById((*itr).owner);
            if (owner)
                owner->m_activeBombs--;

            if (bombDists[DIR_X_POS])
            {
                for (i = 1; i <= bombDists[DIR_X_POS]; i++)
                {
                    df.x = (*itr).x + i;
                    df.y = (*itr).y;
                    df.activeSince = clock() + i*100 + 2500;
                    df.activeTime = 800;
                    df.registered = false;
                    df.originalOwner = (*itr).owner;
                    m_dangerousMap.push_back(df);
                }
            }

            if (bombDists[DIR_X_NEG])
            {
                for (i = 1; i <= bombDists[DIR_X_NEG]; i++)
                {
                    df.x = (*itr).x - i;
                    df.y = (*itr).y;
                    df.activeSince = clock() + i*100 + 2500;
                    df.activeTime = 800;
                    df.registered = false;
                    df.originalOwner = (*itr).owner;
                    m_dangerousMap.push_back(df);
                }
            }

            if (bombDists[DIR_Y_POS])
            {
                for (i = 1; i <= bombDists[DIR_Y_POS]; i++)
                {
                    df.x = (*itr).x;
                    df.y = (*itr).y + i;
                    df.activeSince = clock() + i*100 + 2500;
                    df.activeTime = 800;
                    df.registered = false;
                    df.originalOwner = (*itr).owner;
                    m_dangerousMap.push_back(df);
                }
            }

            if (bombDists[DIR_Y_NEG])
            {
                for (i = 1; i <= bombDists[DIR_Y_NEG]; i++)
                {
                    df.x = (*itr).x;
                    df.y = (*itr).y - i;
                    df.activeSince = clock() + i*100 + 2500;
                    df.activeTime = 800;
                    df.registered = false;
                    df.originalOwner = (*itr).owner;
                    m_dangerousMap.push_back(df);
                }
            }

            df.x = (*itr).x;
            df.y = (*itr).y;
            df.activeSince = clock() + 2500;
            df.activeTime = 800;
            df.registered = false;
            df.originalOwner = (*itr).owner;
            m_dangerousMap.push_back(df);

            for (std::list<DynamicRecord>::iterator iter = m_dynRecords.begin(); iter != m_dynRecords.end(); ++iter)
            {
                if ((*iter).x == (*itr).x && (*iter).y == (*itr).y && (*iter).type == DYNAMIC_TYPE_BOMB)
                {
                    m_dynRecords.erase(iter);
                    break;
                }
            }

            itr = m_bombMap.erase(itr);
            continue;
        }
    }

    for (std::list<DangerousField>::iterator itr = m_dangerousMap.begin(); itr != m_dangerousMap.end(); )
    {
        if (!(*itr).registered && (*itr).activeSince >= clock())
        {
            (*itr).registered = true;

            for (i = 0; i < MAX_PLAYERS_PER_INSTANCE; i++)
            {
                if (pPlayers[i] && pPlayers[i]->m_respawnTime == 0 && ceil(fabs(pPlayers[i]->m_positionX)) == (*itr).x && ceil(fabs(pPlayers[i]->m_positionY)) == (*itr).y)
                {
                    SmartPacket die(SMSG_PLAYER_DIED);
                    die << uint32(pPlayers[i]->m_socket);
                    die << float(pPlayers[i]->m_positionX);
                    die << float(pPlayers[i]->m_positionY);
                    die << uint32(DEFAULT_RESPAWN_TIME);
                    sInstanceManager->SendInstancePacket(&die, id, true);

                    pPlayers[i]->m_respawnTime = time(NULL) + DEFAULT_RESPAWN_TIME;

                    // score part
                    if ((*itr).originalOwner)
                    {
                        Player* owner = sSession->GetPlayerById((*itr).originalOwner);
                        if (owner)
                            owner->score.kills += 1;
                    }
                    pPlayers[i]->score.deaths += 1;

                    // and update scoreboard
                    SendScoreBoard();
                }
            }

            for (std::list<DynamicRecord>::iterator iter = m_dynRecords.begin(); iter != m_dynRecords.end(); ++iter)
            {
                if ((*iter).x == (*itr).x && (*iter).y == (*itr).y && (*iter).type == DYNAMIC_TYPE_BOX)
                {
                    SmartPacket box(SMSG_BOX_DESTROYED);
                    box << uint32((*iter).x);
                    box << uint32((*iter).y);
                    sInstanceManager->SendInstancePacket(&box, id, true);

                    m_dynRecords.erase(iter);
                    break;
                }
            }
        }

        if ((*itr).registered && (*itr).activeSince + clock_t((*itr).activeTime) < clock())
            itr = m_dangerousMap.erase(itr);
        else
            ++itr;
    }

    /*m_playerMapLock  = false;
    m_sharedMapsLock = false;*/
}

void Instance::SendScoreBoard()
{
    SmartPacket data(SMSG_SCOREBOARD);

    uint32 plCount = 0;
    for (uint32 i = 0; i < MAX_PLAYERS_PER_INSTANCE; i++)
        if (pPlayers[i] != NULL)
            plCount++;

    data << uint32(plCount);

    for (uint32 i = 0; i < MAX_PLAYERS_PER_INSTANCE; i++)
    {
        if (pPlayers[i] != NULL)
        {
            data << pPlayers[i]->m_nickName.c_str();
            data << uint32(pPlayers[i]->score.kills);
            data << uint32(pPlayers[i]->score.deaths);
        }
    }

    sInstanceManager->SendInstancePacket(&data, id);
}
