#include "data/Leaderboard.h"

#include <fstream>

vector<ScoreEntry> loadScores(const string& path) {
    vector<ScoreEntry> scores;
    ifstream file(path);
    ScoreEntry entry;
    int mode = 0;
    int difficulty = 0;

    while (file >> entry.name >> mode >> difficulty >> entry.size >> entry.seconds >> entry.moves >> entry.score) {
        if (mode < 0 || mode > 2 || difficulty < 0 || difficulty > 2) {
            continue;   // ignore damaged lines
        }
        entry.mode = static_cast<GameMode>(mode);
        entry.difficulty = static_cast<Difficulty>(difficulty);
        scores.push_back(entry);
    }
    return scores;
}

void saveScores(const vector<ScoreEntry>& scores, const string& path) {
    ofstream file(path);
    for (const ScoreEntry& entry : scores) {
        file << entry.name << ' ' << static_cast<int>(entry.mode) << ' ' << static_cast<int>(entry.difficulty) << ' '
            << entry.size << ' ' << entry.seconds << ' ' << entry.moves << ' ' << entry.score << '\n';
    }
}