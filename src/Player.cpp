#include <global.h>
#include "Player.h"

Player::Player() : Client()
{
    for (int i = 0; i < MAX_BONUSES; i++)
        m_bonuses[i] = 0;

    m_activeBombs = 0;

    m_instance = -1;
    m_positionX = 0.0f;
    m_positionY = 0.0f;
    m_orientation = 0.0f;

    m_modelIdOffset = 0;

    m_nickName = "ABCDEFGH";

    m_isMoving = false;
}
