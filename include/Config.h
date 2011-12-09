/*                                         *
*            Bomberman Server              *
*                                          *
*    Developed by: Cmaranec (Kennny)       *
*                                          *
*    Copyright (c) 2011                    *
*                                          */

#ifndef __CONFIG_H_
#define __CONFIG_H_

#include <global.h>
#include "Singleton.h"

#include <iostream>

enum ConfigElemTypes
{
    CONFIG_TYPE_STRING,
    CONFIG_TYPE_INTEGER,
};

enum ConfigStringElements
{
    CONFIG_DB_HOST,
    CONFIG_DB_USER,
    CONFIG_DB_PASSWORD,
    CONFIG_DB_MAIN_DB,

    MAX_CONFIG_STRING_VALUES
};

enum ConfigIntegerElements
{
    CONFIG_DB_PORT,

    CONFIG_MAIN_PORT,

    MAX_CONFIG_INTEGER_VALUES
};

class Config
{
    public:
        Config() {};
        ~Config() {};

        bool LoadConfig(const char* filename);
        void LoadConfigValue(uint8 type, uint32 index, const char* identifier);

        uint32 GetIntValue(uint32 index) { return m_configIntValues[index]; };
        const char* GetStringValue(uint32 index) { return m_configStringValues[index].c_str(); };

    protected:
        uint32 m_configIntValues[MAX_CONFIG_INTEGER_VALUES];
        std::string m_configStringValues[MAX_CONFIG_STRING_VALUES];

        FILE* pConfig;
};

static Config* sConfig = Singleton<Config>::Instance();

#endif
