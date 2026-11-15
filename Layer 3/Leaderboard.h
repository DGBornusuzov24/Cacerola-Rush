#pragma once

#include <vector>
#include "Types.h"

const string SCORE_FILE = "leaderboard.txt";

// The leaderboard is a text file with one game per line:  name mode level size seconds moves score
vector<ScoreEntry> loadScores(const string& path = SCORE_FILE);
void saveScores(const vector<ScoreEntry>& scores, const string& path = SCORE_FILE);
