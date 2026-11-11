#pragma once

#include <string>
#include <vector>
using namespace std;

// Theme colours (256-colour palette numbers)
const int THEME_BACKGROUND = 17;
const int THEME_PANEL = 53;
const int THEME_BORDER = 213;
const int THEME_TEXT = 189;
const int THEME_DIM = 103;
const int THEME_GOLD = 220;
const int THEME_SELECTED = 201;
const int THEME_GOOD = 46;
const int THEME_BAD = 203;
const char* const TEXT_BOLD = "\033[1m";

string fgColor(int code);
string bgColor(int code);
string resetColors();                   // default text colour + the navy background
string moveTo(int row, int column);     // rows and columns start from 1

void initConsole();                     // full-screen mode, call once at the start
void closeConsole();
void showCursor(bool visible);
void getTerminalSize(int& columns, int& rows);

int visibleLength(const string& text);  // length without colour codes
int centeredColumn(int width);

// Paints the starry background and draws every line in the CENTER of the terminal.
// Returns the screen row of the first line.
int drawCentered(const vector<string>& lines);
