#pragma once

#include <string>
#include <vector>
#include "Types.h"
#include "Console.h"
using namespace std;

const int BOX_WIDTH = 54;   // characters between the two borders of a box

string getModeName(GameMode mode);
string getDifficultyName(Difficulty difficulty);
int getDifficultyColor(Difficulty difficulty);
string formatTime(int seconds);   // 125 -> "02:05"

// The big MAZE title and a little snake, shown on top of the menus.
vector<string> buildTitle();
string buildSnakeBanner(int background);

// Rows of a purple box. Every row is exactly BOX_WIDTH + 2 characters wide.
string boxTop();
string boxSeparator();
string boxBottom();
string boxLine(const string& content, int background = THEME_PANEL);
string boxTitle(const string& heading);
string boxText(const string& text);
string boxGlyphText(const string& glyph, const string& text);
string textStyle();

// Shows a box with these rows in the middle of the screen and waits for any key.
void showBox(const vector<string>& rows);
