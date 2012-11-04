/*                                         *
*            Bomberman Server              *
*                                          *
*    Developed by: Cmaranec (Kennny)       *
*                                          *
*    Copyright (c) 2011                    *
*                                          */

#include <global.h>
#include "Log.h"
#include "Application.h"

#ifdef _WIN32
 #include <windows.h>
#else
 #include <stdio.h>
 #include <stdarg.h>
 #include <cstdlib>
#endif

#include <iostream>
#include <cstdarg>
using namespace std;

void Log::StaticOut(const char* c)
{
    cout << c << endl;
}

void Log::StringOut(const char *str, ...)
{
    va_list argList;
    va_start(argList,str);
    char buf[2048];
    vsnprintf(buf,2048,str,argList);
    va_end(argList);
    cout << buf << endl;
}

void Log::StringOutLine(const char *str, ...)
{
    va_list argList;
    va_start(argList,str);
    char buf[2048];
    vsnprintf(buf,2048,str,argList);
    va_end(argList);
    cout << buf;
}

void Log::ErrorOut(const char *str, ...)
{
    va_list argList;
    va_start(argList,str);
    char buf[2048];
    vsnprintf(buf,2048,str,argList);
    va_end(argList);
    SetConsoleColor(1);
    cout << "! Error: ";
    SetConsoleColor(0);
    cout << buf << endl;
}

void Log::DebugOut(const char *str, ...)
{
    if (!sApp->isDebug(DEBUG_SYSTEM))
        return;

    va_list argList;
    va_start(argList,str);
    char buf[2048];
    vsnprintf(buf,2048,str,argList);
    va_end(argList);
    cout << buf << endl;
}

void Log::NetworkOut(Client* pClient, const char *str, ...)
{
    if (!pClient)
    {
        ErrorOut("Invalid (NULL) Client pointer as argument in Log::NetworkOut");
        return;
    }

    va_list argList;
    va_start(argList,str);
    char buf[2048];
    vsnprintf(buf,2048,str,argList);
    va_end(argList);
    cout << "Network: ";
    SetConsoleColor(2);
    cout << pClient->m_socket;
    SetConsoleColor(0);
    cout << ": " << buf << endl;
}

void Log::NetworkDebugOut(Client* pClient, const char *str, ...)
{
    if (!sApp->isDebug(DEBUG_NETWORK))
        return;

    va_list argList;
    va_start(argList,str);
    char buf[2048];
    vsnprintf(buf,2048,str,argList);
    va_end(argList);
    cout << "Network: ";
    SetConsoleColor(2);
    cout << pClient->m_socket;
    SetConsoleColor(0);
    cout << ": " << buf << endl;
}

void Log::init()
{
// Clear screen implemented only for Windows
#ifdef _WIN32
    COORD coordScreen = { 0, 0 };
    DWORD cCharsWritten;
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    DWORD dwConSize;
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

    GetConsoleScreenBufferInfo(hConsole, &csbi);
    dwConSize = csbi.dwSize.X * csbi.dwSize.Y;
    FillConsoleOutputCharacter(hConsole, TEXT(' '),
                             dwConSize,
                             coordScreen,
                             &cCharsWritten);
    GetConsoleScreenBufferInfo(hConsole, &csbi);
    FillConsoleOutputAttribute(hConsole,
                             csbi.wAttributes,
                             dwConSize,
                             coordScreen,
                             &cCharsWritten);
    SetConsoleCursorPosition(hConsole, coordScreen);
#else
    cout << "\033[2J\033[1;1H";
#endif

    SetConsoleColor(0);
}

void Log::SetConsoleColor(int32 color)
{
#ifdef _WIN32
    switch (color)
    {
    case 0:    // White on Black
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
                        FOREGROUND_INTENSITY | FOREGROUND_RED |
                        FOREGROUND_GREEN | FOREGROUND_BLUE);
        break;
    case 1:    // Red on Black
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
                         FOREGROUND_INTENSITY | FOREGROUND_RED);
        break;
    case 2:    // Green on Black
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
                      FOREGROUND_INTENSITY | FOREGROUND_GREEN);
        break;
    case 3:    // Yellow on Black
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
                      FOREGROUND_INTENSITY | FOREGROUND_RED |
                      FOREGROUND_GREEN);
        break;
    case 4:    // Blue on Black
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
                      FOREGROUND_INTENSITY | FOREGROUND_BLUE);
        break;
    case 5:    // Magenta on Black
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
                     FOREGROUND_INTENSITY | FOREGROUND_RED |
                     FOREGROUND_BLUE);
        break;
    case 6:    // Cyan on Black
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
                    FOREGROUND_INTENSITY | FOREGROUND_GREEN |
                    FOREGROUND_BLUE);
        break;
    default:    // White on Black
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
                FOREGROUND_INTENSITY | FOREGROUND_RED |
                FOREGROUND_GREEN | FOREGROUND_BLUE);
        break;
    }
#else
    switch (color)
    {
    case 0:
    default:
        cout << "\033[01;37m";
        break;
    case 1:
        cout << "\033[22;31m";
        break;
    case 2:
        cout << "\033[22;32m";
        break;
    case 3:
        cout << "\033[01;33m";
        break;
    case 4:
        cout << "\033[22;34m";
        break;
    case 5:
        cout << "\033[22;35m";
        break;
    case 6:
        cout << "\033[22;36m";
        break;
    }
#endif
}
