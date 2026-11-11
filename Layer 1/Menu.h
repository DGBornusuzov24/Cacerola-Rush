#pragma once
#pragma once

#include <string>
using namespace std;

// Menus use the Up / Down arrows and Enter.

int showMainMenu();        // 0 = Play, 1 = Leaderboard, 2 = How to play, 3 = Exit
int chooseGameMode();      // 0 = Classic, 1 = Dark Maze, 2 = Move Limit, -1 = Back
int chooseDifficulty();    // 0 = Easy, 1 = Normal, 2 = Hard, -1 = Back

int askMazeSize();         // repeats until the typed size is valid (10-50)
string askPlayerName();    // repeats until the typed name is valid

void showHelp();
void showGoodbye();
