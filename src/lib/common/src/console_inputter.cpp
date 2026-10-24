/*
 * Copyright 2020 McGraw-Hill Education. All rights reserved. No reproduction or distribution without the prior written consent of McGraw-Hill Education.
 *
 * -----------------------------------------------------------
 * Modifications Copyright (c) 2026 François Aucoin.
 * 
 * Notice: This file has been modified from its original version.
 * Description of changes: Added support for Emscripten (Node.js)
 * targets.
 * ----------------------------------------------------------- 
 */

#include <iostream>

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32)
    #include <conio.h>
#else
    #include <termios.h>
    #include <unistd.h>
#endif

#include "console_inputter.h"

#ifdef __EMSCRIPTEN__
extern "C"
{
    // NOTE: Those are called "imported functions" in Emscripten/Wasm jargon.
    // Those function must be provided by the host (e.g., Node.js code).
    void stdin_set_input_state(bool state);
    bool stdin_is_character_available();
    char stdin_get_next_character();
}
#endif

void lc3::ConsoleInputter::beginInput(void)
{
#ifdef __EMSCRIPTEN__
    stdin_set_input_state(true);
#elif !(defined(WIN32) || defined(_WIN32) || defined(__WIN32))
    struct termios ttystate;

    tcgetattr(STDIN_FILENO, &ttystate);
    ttystate.c_lflag &= ~ICANON;
    ttystate.c_lflag &= ~ECHO;
    ttystate.c_cc[VMIN] = 1;

    tcsetattr(STDIN_FILENO, TCSANOW, &ttystate);
#endif
}

bool lc3::ConsoleInputter::getChar(char & c)
{
#ifdef __EMSCRIPTEN__
    if (stdin_is_character_available())
    {
        c = stdin_get_next_character();
        return true;
    }
#elif defined(WIN32) || defined(_WIN32) || defined(__WIN32)
    if(_kbhit() != 0) {
        c = _getch();
        return true;
    }
#else
    if(kbhit() != 0) {
        c = fgetc(stdin);
        return true;
    }
#endif
    return false;
}

void lc3::ConsoleInputter::endInput(void)
{
#ifdef __EMSCRIPTEN__
    stdin_set_input_state(false);
#elif !(defined(WIN32) || defined(_WIN32) || defined(__WIN32))
    struct termios ttystate;
    tcgetattr(STDIN_FILENO, &ttystate);
    ttystate.c_lflag |= ICANON;
    ttystate.c_lflag |= ECHO;
    tcsetattr(STDIN_FILENO, TCSANOW, &ttystate);
#endif
}

#ifdef __EMSCRIPTEN__

#elif !(defined(WIN32) || defined(_WIN32) || defined(__WIN32))
int lc3::ConsoleInputter::kbhit(void)
{
    struct timeval tv;
    fd_set fds;
    tv.tv_sec = 0;
    tv.tv_usec = 0;
    FD_ZERO(&fds);
    FD_SET(STDIN_FILENO, &fds);
    select(STDIN_FILENO+1, &fds, NULL, NULL, &tv);
    return FD_ISSET(STDIN_FILENO, &fds);
}
#endif
