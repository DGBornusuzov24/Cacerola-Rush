#include "Screens.h"

#include "GameRules.h"
#include "Scores.h"
#include "Console.h"
#include "InputHandler.h"
#include "Panel.h"

static string buildStars(int stars) {
    string text;
    for (int i = 0; i < 3; i++) {
        if (i < stars) {
            text += fgColor(THEME_GOLD) + TEXT_BOLD + " * ";
        }
        else {
            text += fgColor(60) + " * ";
        }
    }
    return text;
}

void showWinScreen(const GameState& state, const ScoreEntry& entry, int rank, int bestBefore) {
    string record = "Time to beat: " + formatTime(bestBefore);
    if (isNewRecord(bestBefore, entry.seconds)) {
        record = fgColor(THEME_GOLD) + TEXT_BOLD + ">> NEW TIME RECORD! <<";
    }
    showBox({
        boxTitle("RESULT"),
        boxSeparator(),
        boxLine(""),
        boxLine("               " + buildStars(getStarRating(state))),
        boxLine(""),
        boxText("Mode:       " + getModeName(entry.mode) + " / " + getDifficultyName(entry.difficulty)),
        boxText("Maze size:  " + to_string(entry.size) + " x " + to_string(entry.size)),
        boxText("Time:       " + formatTime(entry.seconds)),
        boxText("Moves:      " + to_string(entry.moves) + "   (best possible: " + to_string(state.bestRoute) + ")"),
        boxText("Hints:      " + to_string(state.hintsUsed)),
        boxText("Score:      " + to_string(entry.score)),
        boxText("Rank:       #" + to_string(rank) + " in " + getModeName(entry.mode)),
        boxLine(""),
        boxText(record),
        boxLine(""),
        boxLine("  " + buildSnakeBanner(THEME_PANEL) + textStyle() + "  Great job, " + entry.name + "!")
        });
}

void showLoseScreen(const GameState& state) {
    showBox({
        boxTitle("OUT OF MOVES!"),
        boxSeparator(),
        boxLine(""),
        boxText("The snake got tired before the exit."),
        boxText("Moves used:  " + to_string(state.moveCount)),
        boxText("Keys found:  " + to_string(state.keysCollected) + " / " + to_string(state.keys.size())),
        boxText("Tip: a hint (H) shows the way to your next goal."),
        boxLine("")
        });
}

void showPauseScreen() {
    showBox({
        boxTitle("PAUSED"),
        boxSeparator(),
        boxLine(""),
        boxText("The timer is stopped. Take a break!"),
        boxLine("")
        });
}

static string buildTabs(int selected) {
    string tabs;
    for (int i = 0; i < MODE_COUNT; i++) {
        string name = " " + getModeName((GameMode)i) + " ";
        if (i == selected) {
            tabs += bgColor(THEME_SELECTED) + fgColor(231) + TEXT_BOLD + name;
        }
        else {
            tabs += fgColor(THEME_DIM) + name;
        }
        tabs += resetColors() + " ";
    }
    return tabs;
}

static string buildScoreRow(int place, const ScoreEntry& entry) {
    const int medalColors[3] = { 220, 252, 208 };   // gold, silver, bronze
    int color = THEME_TEXT;
    if (place < 3) {
        color = medalColors[place];
    }
    return boxLine("  " + fgColor(color) + padVisible("#" + to_string(place + 1), 4) + padVisible(entry.name, 11) +
        fgColor(getDifficultyColor(entry.difficulty)) + padVisible(getDifficultyName(entry.difficulty), 11) +
        fgColor(color) + padVisible(to_string(entry.size) + "x" + to_string(entry.size), 7) +
        padVisible(formatTime(entry.seconds), 7) + padVisible(to_string(entry.moves), 6) + to_string(entry.score));
}

void showLeaderboard(const vector<ScoreEntry>& scores) {
    int tab = 0;
    while (true) {
        vector<ScoreEntry> top = getTopScores(scores, (GameMode)tab, 10);

        vector<string> lines;
        lines.push_back(fgColor(THEME_GOLD) + TEXT_BOLD + "══  L E A D E R B O A R D  ══");
        lines.push_back(" ");
        lines.push_back(buildTabs(tab));
        lines.push_back(" ");
        lines.push_back(boxTop());
        lines.push_back(boxLine("  " + fgColor(THEME_GOLD) + padVisible("#", 4) + padVisible("NAME", 11) + padVisible("LEVEL", 11) +
            padVisible("SIZE", 7) + padVisible("TIME", 7) + padVisible("MOVES", 6) + "SCORE"));
        lines.push_back(boxSeparator());
        for (int i = 0; i < 10; i++) {
            if (i < (int)top.size()) {
                lines.push_back(buildScoreRow(i, top[i]));
            }
            else if (i == 0) {
                lines.push_back(boxText("No scores yet - be the first!"));
            }
            else {
                lines.push_back(boxLine(""));
            }
        }
        lines.push_back(boxBottom());
        lines.push_back(" ");
        lines.push_back(fgColor(THEME_DIM) + "←  →  switch mode      Q  back");
        drawCentered(lines);

        Key key = readKey();
        if (key == Key::Left) {
            tab--;
            if (tab < 0) {
                tab = MODE_COUNT - 1;
            }
        }
        else if (key == Key::Right) {
            tab++;
            if (tab == MODE_COUNT) {
                tab = 0;
            }
        }
        else if (key == Key::Quit || key == Key::Enter) {
            return;
        }
    }
}
