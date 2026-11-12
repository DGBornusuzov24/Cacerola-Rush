#include "presentation/Game.h"

#include "data/Leaderboard.h"
#include "logic/GameRules.h"
#include "logic/PlayerMovement.h"
#include "logic/Scores.h"
#include "presentation/Console.h"
#include "presentation/InputHandler.h"
#include "presentation/MazeDisplay.h"
#include "presentation/Menu.h"
#include "presentation/Screens.h"

namespace {

    bool keyToDirection(Key key, Direction& direction) {
        switch (key) {
        case Key::Up:    direction = Direction::Up;    return true;
        case Key::Down:  direction = Direction::Down;  return true;
        case Key::Left:  direction = Direction::Left;  return true;
        case Key::Right: direction = Direction::Right; return true;
        default:         return false;
        }
    }

    string describeMove(MoveResult result) {
        if (result == MoveResult::Blocked) {
            return "A wall blocks your way!";
        }
        return result == MoveResult::UsedDoor ? "You slithered through a special door!" : "";
    }

    // The player won: ask for a name, save the score and show the result.
    void finishWonGame(const GameState& state, int seconds, int bestTime) {
        vector<ScoreEntry> scores = loadScores();
        ScoreEntry entry = makeScoreEntry(state, askPlayerName(), seconds);
        int rank = addScore(scores, entry);
        saveScores(scores);
        showWinScreen(entry, rank, bestTime);
    }

    void playGame() {
        int modeChoice = chooseGameMode();
        int difficultyChoice = modeChoice < 0 ? -1 : chooseDifficulty();
        if (difficultyChoice < 0) {
            return;
        }
        GameMode mode = static_cast<GameMode>(modeChoice);
        Difficulty difficulty = static_cast<Difficulty>(difficultyChoice);
        int size = askMazeSize();

        GameState state;
        startNewGame(state, size, mode, difficulty);
        int bestTime = findBestTime(loadScores(), mode, difficulty, size);
        string message = "Guide the snake to the exit!";
        bool playerQuit = false;

        while (!hasPlayerWon(state) && !hasPlayerLost(state) && !playerQuit) {
            drawGame(state, message, bestTime);

            Key key = readKey(500);   // no key for half a second: just redraw, so the timer keeps running
            Direction direction = Direction::Up;
            if (key == Key::Quit) {
                playerQuit = true;
            }
            else if (keyToDirection(key, direction)) {
                message = describeMove(movePlayer(state, direction));
            }
            else if (key != Key::None) {
                message = "Use the arrow keys or W A S D.";
            }
        }

        if (hasPlayerWon(state)) {
            finishWonGame(state, getElapsedSeconds(state), bestTime);
        }
        else if (hasPlayerLost(state)) {
            showLoseScreen(state);
        }
        endGame(state);
    }

}  // namespace

void runApplication() {
    initConsole();
    startKeyboard();

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
    stopKeyboard();
    closeConsole();
}
