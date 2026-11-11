// System headers come first: <windows.h> does not work after "using namespace std".
#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#ifndef ENABLE_VIRTUAL_TERMINAL_PROCESSING
#define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004
#endif
#else
#include <sys/ioctl.h>
#include <unistd.h>
#endif
#include <cctype>
#include <iostream>

#include "presentation/Console.h"

namespace {

// Navy sky with stars. The stars come from a fixed formula, so they stay in place and never flicker.
string buildBackground(int columns, int rows) {
    string screen = bgColor(THEME_BACKGROUND);
    for (int row = 1; row <= rows; row++) {
        screen += moveTo(row, 1);
        int lastColumn = (row == rows) ? columns - 1 : columns;   // the very last cell would scroll the screen
        for (int col = 1; col <= lastColumn; col++) {
            unsigned int hash = static_cast<unsigned int>(row) * 73856093u ^ static_cast<unsigned int>(col) * 19349663u;
            hash = (hash ^ (hash >> 7)) * 2654435761u;
            unsigned int luck = (hash >> 8) % 150;
            screen += (luck == 0) ? fgColor(60) + "·" : (luck == 1) ? fgColor(104) + "*" : " ";
        }
    }
    return screen;
}

}  // namespace

string fgColor(int code) {
    return "\033[38;5;" + to_string(code) + "m";
}

string bgColor(int code) {
    return "\033[48;5;" + to_string(code) + "m";
}

string resetColors() {
    return "\033[0m" + bgColor(THEME_BACKGROUND);
}

string moveTo(int row, int column) {
    return "\033[" + to_string(row) + ";" + to_string(column) + "H";
}

void initConsole() {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    DWORD mode = 0;
    HANDLE output = GetStdHandle(STD_OUTPUT_HANDLE);
    if (GetConsoleMode(output, &mode)) {
        SetConsoleMode(output, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
    }
#endif
    cout << "\033[?1049h";   // the old terminal content comes back when we leave
    showCursor(false);
}

void closeConsole() {
    showCursor(true);
    cout << "\033[0m\033[?1049l" << flush;
}

void showCursor(bool visible) {
    cout << (visible ? "\033[?25h" : "\033[?25l") << flush;
}

void getTerminalSize(int& columns, int& rows) {
    columns = 120;   // used when the size cannot be read
    rows = 40;
#ifdef _WIN32
    CONSOLE_SCREEN_BUFFER_INFO info;
    if (GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &info)) {
        columns = info.srWindow.Right - info.srWindow.Left + 1;
        rows = info.srWindow.Bottom - info.srWindow.Top + 1;
    }
#else
    winsize size;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &size) == 0 && size.ws_col > 0 && size.ws_row > 0) {
        columns = size.ws_col;
        rows = size.ws_row;
    }
#endif
}

int visibleLength(const string& text) {
    int length = 0;
    for (size_t i = 0; i < text.size(); i++) {
        if (text[i] == '\033') {
            while (i < text.size() && !isalpha(static_cast<unsigned char>(text[i]))) {
                i++;   // skip a colour code like ESC[38;5;213m
            }
        } else if ((static_cast<unsigned char>(text[i]) & 0xC0) != 0x80) {
            length++;  // UTF-8 symbols use several bytes but are only one character
        }
    }
    return length;
}

int centeredColumn(int width) {
    int columns = 0;
    int rows = 0;
    getTerminalSize(columns, rows);
    return max(1, (columns - width) / 2 + 1);
}

int drawCentered(const vector<string>& lines) {
    int columns = 0;
    int rows = 0;
    getTerminalSize(columns, rows);
    int top = max(1, (rows - static_cast<int>(lines.size())) / 2 + 1);

    string screen = buildBackground(columns, rows);
    for (size_t i = 0; i < lines.size(); i++) {
        screen += moveTo(top + static_cast<int>(i), centeredColumn(visibleLength(lines[i]))) + lines[i] + resetColors();
    }
    cout << screen << flush;   // one single write: no flicker
    return top;
}
