/*                                         *
*            Bomberman Server              *
*                                          *
*    Developed by: Cmaranec (Kennny)       *
*                                          *
*    Copyright (c) 2011                    *
*                                          */

#include <global.h>
#include "Singleton.h"
#include "Application.h"

#include <iostream>
using namespace std;

int main()
{
    sApp->init();
    sApp->run();
    return 0;
}
