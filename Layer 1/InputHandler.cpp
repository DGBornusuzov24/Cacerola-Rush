// <conio.h> and <windows.h> must come before the other includes: they do not work after "using namespace std".
#include <conio.h>
#include <windows.h>

#include <cctype>
#include <iostream>

#include "Console.h"
#include "InputHandler.h"

// An arrow key sends two codes: first 224 (or 0), then one of these.
const int ARROW_UP = 72;
const int ARROW_DOWN = 80;
const int ARROW_LEFT = 75;
const int ARROW_RIGHT = 77;

static Key letterToKey(int letter) {
    switch (tolower(letter)) {
    case 'w':  return Key::Up;
    case 's':  return Key::Down;
    case 'a':  return Key::Left;
    case 'd':  return Key::Right;
    case 'q':  return Key::Quit;
    case 'h':  return Key::Hint;
    case 'p':  return Key::Pause;
    case '\r': return Key::Enter;
    default:   return Key::Unknown;
    }
}

static Key arrowToKey(int code) {
    switch (code) {
    case ARROW_UP:    return Key::Up;
    case ARROW_DOWN:  return Key::Down;
    case ARROW_LEFT:  return Key::Left;
    case ARROW_RIGHT: return Key::Right;
    default:          return Key::Unknown;
    }
}

Key readKey(int timeoutMs) {
    int waited = 0;
    while (!_kbhit()) {   // _kbhit() tells us if a key is waiting
        if (timeoutMs >= 0 && waited >= timeoutMs) {
            return Key::None;
        }
        Sleep(20);
        waited += 20;
    }

    int code = _getch();   // _getch() reads one key without Enter and without printing it
    if (code == 0 || code == 224) {
        return arrowToKey(_getch());
    }
    return letterToKey(code);
}

bool readTextLine(string& text) {
    showCursor(true);
    bool success = (bool)getline(cin, text);
    showCursor(false);
    return success;
}
