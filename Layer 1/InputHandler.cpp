#ifdef _WIN32
#include <conio.h>
#include <windows.h>
#else
#include <poll.h>
#include <termios.h>
#include <unistd.h>
#endif
#include <cctype>
#include <iostream>

#include "presentation/Console.h"
#include "presentation/InputHandler.h"

namespace {

    Key characterToKey(int character) {
        switch (tolower(character)) {
        case 'w': return Key::Up;
        case 's': return Key::Down;
        case 'a': return Key::Left;
        case 'd': return Key::Right;
        case 'q': return Key::Quit;
        case '\r':
        case '\n': return Key::Enter;
        default:  return Key::Unknown;
        }
    }

}  // namespace

#ifdef _WIN32

void startKeyboard() {}
void stopKeyboard() {}

Key readKey(int timeoutMs) {
    for (int waited = 0; !_kbhit(); waited += 20) {
        if (timeoutMs >= 0 && waited >= timeoutMs) {
            return Key::None;
        }
        Sleep(20);
    }
    int code = _getch();
    if (code != 0 && code != 224) {
        return characterToKey(code);
    }
    switch (_getch()) {   // arrow keys send two codes
    case 72: return Key::Up;
    case 80: return Key::Down;
    case 75: return Key::Left;
    case 77: return Key::Right;
    default: return Key::Unknown;
    }
}

#else

namespace {
    termios normalSettings = termios();
}

void startKeyboard() {
    tcgetattr(STDIN_FILENO, &normalSettings);
    termios rawSettings = normalSettings;
    rawSettings.c_lflag &= ~(ICANON | ECHO);   // raw mode: keys arrive at once and are not printed
    tcsetattr(STDIN_FILENO, TCSANOW, &rawSettings);
}

void stopKeyboard() {
    tcsetattr(STDIN_FILENO, TCSANOW, &normalSettings);
}

Key readKey(int timeoutMs) {
    pollfd input = { STDIN_FILENO, POLLIN, 0 };
    unsigned char code = 0;
    if (poll(&input, 1, timeoutMs) <= 0) {
        return Key::None;
    }
    if (read(STDIN_FILENO, &code, 1) <= 0) {
        return Key::Quit;   // input closed
    }
    if (code != 27) {
        return characterToKey(code);
    }
    // An arrow key sends 3 bytes: ESC [ A/B/C/D
    unsigned char rest[2] = { 0, 0 };
    if (poll(&input, 1, 50) <= 0 || read(STDIN_FILENO, rest, 2) != 2 || rest[0] != '[') {
        return Key::Unknown;
    }
    switch (rest[1]) {
    case 'A': return Key::Up;
    case 'B': return Key::Down;
    case 'C': return Key::Right;
    case 'D': return Key::Left;
    default:  return Key::Unknown;
    }
}

#endif

bool readTextLine(string& text) {
    stopKeyboard();   // typing a whole line needs the normal terminal mode
    showCursor(true);
    bool success = static_cast<bool>(getline(cin, text));
    showCursor(false);
    startKeyboard();
    return success;
}
