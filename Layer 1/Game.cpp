#include "Game.h"

#include <ctime>

#include "Leaderboard.h"
#include "GameRules.h"
#include "PlayerMovement.h"
#include "Scores.h"
#include "Console.h"
#include "InputHandler.h"
#include "MazeDisplay.h"
#include "Menu.h"
#include "Screens.h"

static bool keyToDirection(Key key, Direction& direction) {
    switch (key) {
    case Key::Up:    direction = Direction::Up;    return true;
    case Key::Down:  direction = Direction::Down;  return true;
    case Key::Left:  direction = Direction::Left;  return true;
    case Key::Right: direction = Direction::Right; return true;
    default:         return false;
    }
}

static string describeMove(MoveResult result) {
    switch (result) {
    case MoveResult::Blocked:    return "A wall blocks your way!";
    case MoveResult::Locked:     return "This gate is locked. Find a key first!";
    case MoveResult::PickedKey:  return "You found a key!";
    case MoveResult::OpenedGate: return "Click! The gate opens.";
    case MoveResult::UsedDoor:   return "You slithered through a secret door!";
    default:                     return "";
    }
}

static void showCountdown(const GameState& state, int bestTime) {
    const string steps[4] = { "Get ready...  3", "Get ready...  2", "Get ready...  1", "GO!" };
    for (int i = 0; i < 4; i++) {
        drawGame(state, steps[i], bestTime, vector<Position>());
        waitMilliseconds(600);
    }
}

static void pauseGame(GameState& state) {
    time_t pauseStart = time(nullptr);
    showPauseScreen();
    continueAfterPause(state, (int)(time(nullptr) - pauseStart));
}

// The player won: ask for a name, save the score and show the result.
static void finishWonGame(const GameState& state, int bestTime) {
    int seconds = getElapsedSeconds(state);
    drawGame(state, "YOU MADE IT!", bestTime, vector<Position>());
    waitMilliseconds(1200);

    vector<ScoreEntry> scores = loadScores();
    ScoreEntry entry = makeScoreEntry(state, askPlayerName(), seconds);
    int rank = addScore(scores, entry);
    saveScores(scores);
    showWinScreen(state, entry, rank, bestTime);
}

static void playGame() {
    int modeChoice = chooseGameMode();
    if (modeChoice < 0) {
        return;
    }
    GameMode mode = (GameMode)modeChoice;
    int levelChoice = chooseDifficulty(mode);
    if (levelChoice < 0) {
        return;
    }
    Difficulty difficulty = (Difficulty)levelChoice;
    int size = askMazeSize();

    GameState state;
    startNewGame(state, size, mode, difficulty);
    int bestTime = findBestTime(loadScores(), mode, difficulty, size);
    vector<Position> hint;
    string message = "Find the exit!";
    if (!state.keys.empty()) {
        message = "Find the keys, open the gates, reach the exit!";
    }

    showCountdown(state, bestTime);
    restartTimer(state);

    bool playerQuit = false;
    while (!hasPlayerWon(state) && !hasPlayerLost(state) && !playerQuit) {
        drawGame(state, message, bestTime, hint);

        Key key = readKey(500);   // no key for half a second: just redraw, so the timer keeps running
        Direction direction = Direction::Up;
        if (key == Key::None) {
            continue;
        }

        if (key == Key::Quit) {
            playerQuit = true;
        }
        else if (key == Key::Pause) {
            pauseGame(state);
        }
        else if (key == Key::Hint) {
            hint = useHint(state);
            message = "Follow the glowing path!  (+5 seconds)";
            if (hint.empty()) {
                message = "No hints left!";
            }
        }
        else if (keyToDirection(key, direction)) {
            bool wasReversed = areControlsReversed(state);
            MoveResult result = movePlayer(state, direction);
            message = describeMove(result);
            if (result == MoveResult::PickedKey || result == MoveResult::OpenedGate) {
                hint.clear();   // the goal changed, so the old hint is not useful anymore
            }
            if (areControlsReversed(state) != wasReversed) {
                message = "CHAOS! The controls just flipped!";
            }
        }
        else {
            message = "Use the arrows or W A S D.  H = hint, P = pause, Q = quit";
        }
    }

    if (hasPlayerWon(state)) {
        finishWonGame(state, bestTime);
    }
    else if (hasPlayerLost(state)) {
        drawGame(state, "Out of moves...", bestTime, hint);
        waitMilliseconds(1200);
        showLoseScreen(state);
    }
    endGame(state);
}

void runApplication() {
    initConsole();

    bool running = true;
    while (running) {
        int choice = showMainMenu();
        if (choice == 0) {
            playGame();
        }
        else if (choice == 1) {
            showLeaderboard(loadScores());
        }
        else if (choice == 2) {
            showHelp();
        }
        else {
            running = false;
        }
    }

    showGoodbye();
    closeConsole();
}
