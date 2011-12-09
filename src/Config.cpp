/*                                         *
*            Bomberman Server              *
*                                          *
*    Developed by: Cmaranec (Kennny)       *
*                                          *
*    Copyright (c) 2011                    *
*                                          */

#include <global.h>
#include "Config.h"
#include "Log.h"

#include <sstream>

#ifndef _WIN32
 #include <stdio.h>
 #include <stdlib.h>
#endif

bool Config::LoadConfig(const char* filename)
{
    char buf[256];
    sprintf(buf,"%s%s%s",CONF_PATH,PATH_DIR,filename);
    sLog->StringOut("Using main config path: %s",buf);

    pConfig = fopen(buf,"r");
    if (!pConfig)
        return false;

    LoadConfigValue(CONFIG_TYPE_STRING,  CONFIG_DB_HOST,      "DB.Host");
    LoadConfigValue(CONFIG_TYPE_STRING,  CONFIG_DB_USER,      "DB.User");
    LoadConfigValue(CONFIG_TYPE_STRING,  CONFIG_DB_PASSWORD,  "DB.Password");
    LoadConfigValue(CONFIG_TYPE_INTEGER, CONFIG_DB_PORT,      "DB.Port");
    LoadConfigValue(CONFIG_TYPE_STRING,  CONFIG_DB_MAIN_DB,   "DB.MainDB");
    LoadConfigValue(CONFIG_TYPE_INTEGER, CONFIG_MAIN_PORT,    "MainPort");

    return true;
}

void Config::LoadConfigValue(uint8 type, uint32 index, const char *identifier)
{
    if (!pConfig)
        return;

    rewind(pConfig);

    int8 ch = 0;
    char buffer[1024];
    uint32 cnt = 0;
    bool comment = false;
    bool value = false;
    bool valdone = false;

    std::string ident;
    std::string val;

    while(ch != EOF)
    {
        ch = fgetc(pConfig);
        if (ch == '#')
            comment = true;
        if (ch == ' ' && (!value || type != CONFIG_TYPE_STRING))
            continue;
        if (ch == '"')
        {
            if (value)
            {
                buffer[cnt] = '\0';
                val = buffer;
                valdone = true;
            }
            value = !value;
            continue;
        }
        if (ch == '=')
        {
            buffer[cnt] = '\0';
            ident = buffer;
            cnt = 0;
            if (type == CONFIG_TYPE_INTEGER && strcmp(ident.c_str(),identifier) == 0)
                value = true;
            continue;
        }

        if (!comment || !valdone)
        {
            buffer[cnt] = ch;
            cnt++;
        }

        if (ch == '\n' || ch == EOF)
        {
            if (value && type == CONFIG_TYPE_INTEGER)
            {
                buffer[cnt-1] = '\0';
                val = buffer;
                valdone = true;
            }
            if (!comment)
            {
                if (strcmp(identifier,ident.c_str()) == 0)
                {
                    if (type == CONFIG_TYPE_STRING)
                        m_configStringValues[index] = val.c_str();
                    else if (type == CONFIG_TYPE_INTEGER)
                        m_configIntValues[index] = atoi(val.c_str());
                    return;
                }
            }
            comment = false;
            valdone = false;
            cnt = 0;
        }
    }
}
