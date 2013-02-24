#ifndef INSTANCE_H
#define INSTANCE_H

#include "Singleton.h"
#include "Player.h"
#include "Packets/SmartPacket.h"
#include <ctime>

#include <map>

#define MAX_INSTANCES 10
#define MAX_PLAYERS_PER_INSTANCE 4

#define DEFAULT_RESPAWN_TIME 5    // 5 seconds to respawn
#define DEFAULT_BONUS_DELAY  30   // 30 seconds to spawn bonus
#define DEFAULT_BONUS_DURATION 60 // 60 seconds duration

class Instance
{
public:
    Instance()
    {
        id = -1;
        for (int i = 0; i < MAX_PLAYERS_PER_INSTANCE; i++)
            pPlayers[i] = NULL;

        m_sharedMapsLock = false;
        m_playerMapLock = false;
    };
    bool AddBomb(uint32 x, uint32 y, uint32 owner, uint32 reach);
    void Update();
    void SendScoreBoard();
    int32 id;
    std::string instanceName;
    uint32 mapId;
    uint32 players;
    uint32 maxplayers;

    time_t nextBonusTime; // cas, kdy se objevi dalsi bonus
    uint32 bonusDelay;    // cas mezi bonusy

    // Sdileny zamek pro vsechny mapy! Ve vetsine pripadu by se stejne jednalo o zamceni vsech najednou
    bool m_sharedMapsLock;
    // Zamek pro pole hracu
    bool m_playerMapLock;

    struct DynamicRecord
    {
        DynamicRecord()
        {
            x = 0;
            y = 0;
            type = 0;
            misc = 0;
            endingTime = 0;
        };

        uint32 x;
        uint32 y;
        uint8 type;
        uint8 misc;
        time_t endingTime;
    };
    std::list<DynamicRecord> m_dynRecords;
    void GenerateRandomDynamicRecords();
    enum Direction
    {
        DIR_X_POS = 0,
        DIR_Y_POS = 1,
        DIR_X_NEG = 2,
        DIR_Y_NEG = 3
    };
    uint32 GetBombingDistanceInDir(uint32 x, uint32 y, uint32 reach, uint8 dir);

    struct DangerousField
    {
        uint32  x, y;
        clock_t activeSince;
        uint32  activeTime;
        bool    registered;
        uint32  originalOwner;
    };
    std::list<DangerousField> m_dangerousMap;

    struct BombRecord
    {
        uint32 owner;
        uint32 x, y;
        uint32 reach;
        clock_t boomTime;
    };
    std::list<BombRecord> m_bombMap;

    Player* pPlayers[MAX_PLAYERS_PER_INSTANCE];
};

class InstanceManager
{
    public:
        InstanceManager();
        ~InstanceManager();

        bool Initialize();
        bool InitDefaultInstance();
        int32 InitNewInstance();

        void Update();

        void RegisterPlayer(Player* pPlayer, uint32 instanceId);
        int32 GetPlayerInstanceId(Player* pPlayer);
        Instance* GetPlayerInstance(Player* pPlayer);
        void SendInstancePacket(SmartPacket* data, uint32 instanceId, bool alreadyLocked = false);

        void RemovePlayerFromInstances(Player* pPlayer);

        const char* GetInstanceString();

    private:
        std::map<uint32, Instance*> m_Instances;
};

static InstanceManager* sInstanceManager = Singleton<InstanceManager>::Instance();

#endif
