#include "presentation/Screens.h"

#include "logic/Scores.h"
#include "presentation/Console.h"
#include "presentation/InputHandler.h"
#include "presentation/Panel.h"

namespace {

    const int RANK_COLORS[3] = { 220, 252, 208 };   // gold, silver, bronze

    string buildTabs(int selectedTab) {
        string tabs;
        for (int i = 0; i < 3; i++) {
            string name = " " + getModeName(static_cast<GameMode>(i)) + " ";
            tabs += (i == selectedTab ? bgColor(THEME_SELECTED) + fgColor(231) + TEXT_BOLD : fgColor(THEME_DIM)) + name + resetColors() + "  ";
        }
        return tabs;
    }

    string buildScoreRow(int index, const ScoreEntry& entry) {
        int color = index < 3 ? RANK_COLORS[index] : THEME_TEXT;
        return panelLine("  " + fgColor(color) + padRight("#" + to_string(index + 1), 4) + padRight(entry.name, 11) +
            fgColor(getDifficultyColor(entry.difficulty)) + padRight(getDifficultyName(entry.difficulty), 8) +
            fgColor(color) + padRight(to_string(entry.size) + "x" + to_string(entry.size), 7) +
            padRight(formatTime(entry.seconds), 6) + padRight(to_string(entry.moves), 6) + to_string(entry.score));
    }

}  // namespace

void showWinScreen(const ScoreEntry& entry, int rank, int bestBefore) {
    string record = isNewRecord(bestBefore, entry.seconds)
        ? fgColor(THEME_GOLD) + TEXT_BOLD + ">> NEW TIME RECORD! <<"
        : "Time to beat: " + formatTime(bestBefore);
    showBox({
        panelTitle("RESULT"),
        panelSeparator(),
        panelLine(""),
        panelText("Mode:       " + getModeName(entry.mode) + " / " + getDifficultyName(entry.difficulty)),
        panelText("Maze size:  " + to_string(entry.size) + " x " + to_string(entry.size)),
        panelText("Time:       " + formatTime(entry.seconds)),
        panelText("Moves:      " + to_string(entry.moves)),
        panelText("Score:      " + to_string(entry.score)),
        panelText("Rank:       #" + to_string(rank) + " in " + getModeName(entry.mode)),
        panelLine(""),
        panelText(record),
        panelLine(""),
        panelLine("  " + buildSnakeBanner(THEME_PANEL) + textStyle() + "  Great job!")
        });
}

void showLoseScreen(const GameState& state) {
    showBox({
        panelTitle("OUT OF MOVES!"),
        panelSeparator(),
        panelLine(""),
        panelText("The snake got tired before the exit."),
        panelText("Moves used:  " + to_string(state.moveCount)),
        panelText("Tip: doors are shortcuts!"),
        panelLine("")
        });
}

void showLeaderboard(const vector<ScoreEntry>& scores) {
    int tab = 0;
    while (true) {
        vector<ScoreEntry> top = getTopScores(scores, static_cast<GameMode>(tab), 10);

        vector<string> lines = { fgColor(THEME_GOLD) + TEXT_BOLD + "══  L E A D E R B O A R D  ══", " ", buildTabs(tab), " ",
                                panelTop(),
                                panelLine("  " + fgColor(THEME_GOLD) + padRight("#", 4) + padRight("NAME", 11) + padRight("LEVEL", 8) +
                                          padRight("SIZE", 7) + padRight("TIME", 6) + padRight("MOVES", 6) + "SCORE"),
                                panelSeparator() };
        for (int i = 0; i < 10; i++) {
            if (i < static_cast<int>(top.size())) {
                lines.push_back(buildScoreRow(i, top[i]));
            }
            else {
                lines.push_back(panelLine(i == 0 && top.empty() ? textStyle() + "  No scores yet - be the first!" : ""));
            }
        }
        lines.insert(lines.end(), { panelBottom(), " ", fgColor(THEME_DIM) + "←  →  switch mode      Q  back" });
        drawCentered(lines);

        Key key = readKey();
        if (key == Key::Left) {
            tab = (tab + 2) % 3;
        }
        else if (key == Key::Right) {
            tab = (tab + 1) % 3;
        }
        else if (key == Key::Quit || key == Key::Enter) {
            return;
        }
    }
}
