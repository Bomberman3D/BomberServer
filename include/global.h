/*                                         *
*            Bomberman Server              *
*                                          *
*    Developed by: Cmaranec (Kennny)       *
*                                          *
*    Copyright (c) 2011                    *
*                                          */

#ifndef __GLOBAL_H_
#define __GLOBAL_H_

#include <iostream>

#include "Shared/Shared.h"

typedef long int64;
typedef int int32;
typedef short int16;
typedef char int8;
typedef unsigned long uint64;
typedef unsigned int uint32;
typedef unsigned short uint16;
typedef unsigned char uint8;

#pragma warning(disable:4996) // This function or variable may be unsafe
#pragma warning(disable:4244) // Conversion from T1 to T2, possible loss of data

#ifdef _WIN32
 #define PATH_DIR "\\"
 #define CLOCK_MOD 1
#else
 #define PATH_DIR "/"
 #define CLOCK_MOD CLOCKS_PER_SEC
#endif

#ifdef _DEBUG
 #ifdef _WIN32
  #define DATA_PATH "..\\..\\data"
  #define CONF_PATH "..\\..\\config"
 #else
  #define DATA_PATH "../../data"
  #define CONF_PATH "../../config"
 #endif
#else
 #ifdef _WIN32
  #define DATA_PATH ".\\data"
  #define CONF_PATH "."
 #else
  #define DATA_PATH "./data"
  #define CONF_PATH "."
 #endif
#endif

#endif
