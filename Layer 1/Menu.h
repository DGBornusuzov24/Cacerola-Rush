#pragma once

#include <string>
#include "Types.h"
using namespace std;

// The menus use the Up / Down arrows and Enter. Q means "Back".

int showMainMenu();                     // 0 = Play, 1 = Leaderboard, 2 = How to play, 3 = Exit
int chooseGameMode();                   // a GameMode number, or -1 for Back
int chooseDifficulty(GameMode mode);    // a Difficulty number, or -1 for Back

int askMazeSize();        // repeats until the typed size is valid (10-50)
string askPlayerName();   // repeats until the typed name is valid

void showHelp();
void showGoodbye();
