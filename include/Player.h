#ifndef PLAYER_H
#define PLAYER_H

#include "Session.h"

// Enumerator bonusu
// Zatim jen 3 bonusy, implementovano bude pozdeji
enum Bonuses
{
    BONUS_SPEED = 0,
    BONUS_BOMB,
    BONUS_FLAME,
    MAX_BONUSES
};

struct Player: public Client
{
    Player();

    int32 m_instance;
    float m_positionX;
    float m_positionY;
    float m_orientation;

    bool m_isMoving;

    uint8 m_modelIdOffset;

    // Pocitadlo pro kazdy bonus - kolikrat byl sebran
    uint8 m_bonuses[MAX_BONUSES];

    uint32 m_activeBombs;

    std::string m_nickName;

    struct t_Score
    {
        uint32 deaths;
        uint32 kills;
    } score;
};

#endif
