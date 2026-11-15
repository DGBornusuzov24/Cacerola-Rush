// <windows.h> must come before the other includes: it does not work after "using namespace std".
#include <windows.h>

#include <algorithm>
#include <cctype>
#include <iostream>

#include "Console.h"

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
    SetConsoleOutputCP(CP_UTF8);   // for the block symbols
    HANDLE output = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD mode = 0;
    GetConsoleMode(output, &mode);
    SetConsoleMode(output, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);   // turn on the colour codes

    cout << "\033[?1049h";   // own screen: the old window content comes back at the end
    showCursor(false);
}

void closeConsole() {
    showCursor(true);
    cout << "\033[0m\033[?1049l" << flush;
}

void showCursor(bool visible) {
    if (visible) {
        cout << "\033[?25h" << flush;
    }
    else {
        cout << "\033[?25l" << flush;
    }
}

void getTerminalSize(int& columns, int& rows) {
    CONSOLE_SCREEN_BUFFER_INFO info;
    if (GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &info)) {
        columns = info.srWindow.Right - info.srWindow.Left + 1;
        rows = info.srWindow.Bottom - info.srWindow.Top + 1;
    }
    else {
        columns = 120;   // used when the size cannot be read
        rows = 30;
    }
}

void waitMilliseconds(int milliseconds) {
    Sleep(milliseconds);
}

int visibleLength(const string& text) {
    int length = 0;
    bool insideColorCode = false;
    for (char letter : text) {
        unsigned char code = (unsigned char)letter;
        if (code == 27) {
            insideColorCode = true;     // ESC starts a colour code like ESC[38;5;213m
        }
        else if (insideColorCode) {
            if (isalpha(code)) {
                insideColorCode = false;   // ...and a letter ends it
            }
        }
        else if (code < 128 || code >= 192) {
            length++;   // a symbol like █ has several bytes; only its first byte is counted
        }
    }
    return length;
}

string padVisible(const string& text, int width) {
    int missing = width - visibleLength(text);
    if (missing < 0) {
        missing = 0;
    }
    return text + string(missing, ' ');
}

string repeatText(const string& text, int times) {
    string result;
    for (int i = 0; i < times; i++) {
        result += text;
    }
    return result;
}

int centeredColumn(int width) {
    int columns = 0;
    int rows = 0;
    getTerminalSize(columns, rows);
    return max(1, (columns - width) / 2 + 1);
}

// Navy sky with a few stars. The formula always gives the same places, so the stars never flicker.
static string buildBackground(int columns, int rows) {
    string screen = bgColor(THEME_BACKGROUND);
    for (int row = 1; row <= rows; row++) {
        screen += moveTo(row, 1);
        int lastColumn = columns;
        if (row == rows) {
            lastColumn = columns - 1;   // writing the very last cell would scroll the screen
        }
        for (int col = 1; col <= lastColumn; col++) {
            int star = (row * 37 + col * 91 + row * col) % 113;
            if (star == 0) {
                screen += fgColor(104) + "*";
            }
            else if (star == 1) {
                screen += fgColor(60) + "·";
            }
            else {
                screen += " ";
            }
        }
    }
    return screen;
}

int drawCentered(const vector<string>& lines) {
    int columns = 0;
    int rows = 0;
    getTerminalSize(columns, rows);
    int top = max(1, (rows - (int)lines.size()) / 2 + 1);

    string screen = buildBackground(columns, rows);
    for (int i = 0; i < (int)lines.size(); i++) {
        screen += moveTo(top + i, centeredColumn(visibleLength(lines[i]))) + lines[i] + resetColors();
    }
    cout << screen << flush;   // one single write: no flicker
    return top;
}
