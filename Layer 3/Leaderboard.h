#pragma once

#include <vector>
#include "data/Types.h"

const char* const SCORE_FILE = "leaderboard.txt";

// The leaderboard is saved as a text file: one game per line.
vector<ScoreEntry> loadScores(const string& path = SCORE_FILE);
void saveScores(const vector<ScoreEntry>& scores, const string& path = SCORE_FILE);