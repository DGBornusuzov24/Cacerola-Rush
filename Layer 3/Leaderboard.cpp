#include "Leaderboard.h"

#include <fstream>

vector<ScoreEntry> loadScores(const string& path) {
    vector<ScoreEntry> scores;
    ifstream file(path);
    ScoreEntry entry;
    int mode = 0;
    int level = 0;

    while (file >> entry.name >> mode >> level >> entry.size >> entry.seconds >> entry.moves >> entry.score) {
        bool modeIsValid = mode >= 0 && mode < MODE_COUNT;
        bool levelIsValid = level >= 0 && level <= 3;
        if (modeIsValid && levelIsValid) {
            entry.mode = (GameMode)mode;
            entry.difficulty = (Difficulty)level;
            scores.push_back(entry);
        }
    }
    return scores;
}

void saveScores(const vector<ScoreEntry>& scores, const string& path) {
    ofstream file(path);
    for (const ScoreEntry& entry : scores) {
        file << entry.name << " " << (int)entry.mode << " " << (int)entry.difficulty << " " << entry.size << " "
            << entry.seconds << " " << entry.moves << " " << entry.score << "\n";
    }
}
