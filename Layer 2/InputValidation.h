#pragma once

#include <string>
using namespace std;

// Converts text like " 25 " to 25. Returns false if it is not a whole positive number.
bool tryParseNumber(const string& text, int& number);

// A player name has 1-10 letters, digits or underscores (no spaces).
bool isValidPlayerName(const string& name);