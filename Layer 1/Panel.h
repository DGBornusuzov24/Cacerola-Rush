#pragma once
#include <string>
#include <vector>
#include "data/Types.h"
#include "presentation/Console.h"
using namespace std;

const int PANEL_WIDTH = 52;   // characters between the two borders of a box

string getModeName(GameMode mode);
string getDifficultyName(Difficulty difficulty);
int getDifficultyColor(Difficulty difficulty);
string formatTime(int seconds);                        // 125 -> "02:05"
string padRight(const string& text, int width);

// The big MAZE title + a little snake. Used on top of the menus.
vector<string> buildTitle();
string buildSnakeBanner(int background = THEME_BACKGROUND);   // background = colour behind the tongue

// Pieces of a purple box. Every row is exactly PANEL_WIDTH + 2 characters wide.
string textStyle();
string panelTop();
string panelSeparator();
string panelBottom();
string panelLine(const string& content, int background = THEME_PANEL);
string panelTitle(const string& heading);
string panelText(const string& text);
string panelGlyphText(const string& glyph, const string& text);

// Shows a box with the given rows and waits for any key.
void showBox(const vector<string>& rows);
