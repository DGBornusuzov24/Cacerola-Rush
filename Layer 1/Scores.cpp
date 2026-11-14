#include "logic/Scores.h"

#include <algorithm>

int calculateScore(int size, Difficulty difficulty, int seconds) {
    return size * size * (static_cast<int>(difficulty) + 1) * 1000 / (seconds + 10);
}

ScoreEntry makeScoreEntry(const GameState& state, const string& name, int seconds) {
    ScoreEntry entry;
    entry.name = name;
    entry.mode = state.mode;
    entry.difficulty = state.difficulty;
    entry.size = state.maze.size;
    entry.seconds = seconds;
    entry.moves = state.moveCount;
    entry.score = calculateScore(entry.size, entry.difficulty, seconds);
    return entry;
}

int addScore(vector<ScoreEntry>& scores, const ScoreEntry& entry) {
    scores.push_back(entry);
    int rank = 1;
    for (const ScoreEntry& other : scores) {
        if (other.mode == entry.mode && other.score > entry.score) {
            rank++;
        }
    }
    return rank;
}

vector<ScoreEntry> getTopScores(const vector<ScoreEntry>& scores, GameMode mode, int count) {
    vector<ScoreEntry> top;
    for (const ScoreEntry& entry : scores) {
        if (entry.mode == mode) {
            top.push_back(entry);
        }
    }
    stable_sort(top.begin(), top.end(), [](const ScoreEntry& a, const ScoreEntry& b) { return a.score > b.score; });
    if (static_cast<int>(top.size()) > count) {
        top.resize(count);
    }
    return top;
}

int findBestTime(const vector<ScoreEntry>& scores, GameMode mode, Difficulty difficulty, int size) {
    int best = -1;
    for (const ScoreEntry& entry : scores) {
        if (entry.mode == mode && entry.difficulty == difficulty && entry.size == size &&
            (best == -1 || entry.seconds < best)) {
            best = entry.seconds;
        }
    }
    return best;
}

bool isNewRecord(int bestTime, int seconds) {
    return bestTime == -1 || seconds < bestTime;
}
