#include "pch.h"

#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <queue>
#include <vector>

#include "data/Leaderboard.h"
#include "data/Maze.h"
#include "logic/GameRules.h"
#include "logic/InputValidation.h"
#include "logic/MazeGenerator.h"
#include "logic/PlayerMovement.h"
#include "logic/Scores.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace {

    // A maze made only of walls - each test opens the tiles it needs.
    void prepareEmptyGame(GameState& state, int size, GameMode mode = GameMode::Classic) {
        startNewGame(state, MIN_MAZE_SIZE, mode, Difficulty::Normal);
        allocateMaze(state.maze, size);
        state.player = Position();
        state.tailLength = 0;
        state.moveCount = 0;
    }

    // Breadth-first search that walks only on plain paths (doors count as walls).
    bool isExitReachableWithoutDoors(const Maze& maze) {
        int size = maze.size;
        vector<bool> visited(size * size, false);
        queue<int> waiting;
        waiting.push(0);
        visited[0] = true;
        const int rowStep[4] = { -1, 1, 0, 0 };
        const int colStep[4] = { 0, 0, -1, 1 };

        while (!waiting.empty()) {
            int current = waiting.front();
            waiting.pop();
            if (current == size * size - 1) {
                return true;
            }
            for (int d = 0; d < 4; d++) {
                int row = current / size + rowStep[d];
                int col = current % size + colStep[d];
                if (isInsideMaze(maze, row, col) && !visited[row * size + col] && maze.tiles[row][col] == TileType::Path) {
                    visited[row * size + col] = true;
                    waiting.push(row * size + col);
                }
            }
        }
        return false;
    }

    int countDoors(const Maze& maze) {
        int doors = 0;
        for (int row = 0; row < maze.size; row++) {
            for (int col = 0; col < maze.size; col++) {
                if (maze.tiles[row][col] == TileType::Door) {
                    doors++;
                }
            }
        }
        return doors;
    }

    ScoreEntry makeEntry(const string& name, GameMode mode, Difficulty difficulty, int size, int seconds) {
        ScoreEntry entry;
        entry.name = name;
        entry.mode = mode;
        entry.difficulty = difficulty;
        entry.size = size;
        entry.seconds = seconds;
        entry.moves = 50;
        entry.score = calculateScore(size, difficulty, seconds);
        return entry;
    }

}  // namespace

namespace UnitTest {

    TEST_MODULE_INITIALIZE(SetupRandomSeed) {
        srand(12345);   // fixed seed: the tests give the same result every time
    }

    TEST_CLASS(InputValidationTests) {
public:
    TEST_METHOD(MazeSizeLimits) {
        Assert::IsFalse(isValidMazeSize(9));
        Assert::IsTrue(isValidMazeSize(10));
        Assert::IsTrue(isValidMazeSize(30));
        Assert::IsTrue(isValidMazeSize(50));
        Assert::IsFalse(isValidMazeSize(51));
        Assert::IsFalse(isValidMazeSize(0));
        Assert::IsFalse(isValidMazeSize(-5));
    }

    TEST_METHOD(ParsesValidNumbers) {
        int number = -1;
        Assert::IsTrue(tryParseNumber("15", number));
        Assert::AreEqual(15, number);
        Assert::IsTrue(tryParseNumber("  20  ", number));
        Assert::AreEqual(20, number);
    }

    TEST_METHOD(RejectsInvalidNumbers) {
        int number = 7;
        const char* invalidTexts[] = { "", "   ", "abc", "12a", "-5", "1.5", "99999999999" };
        for (const char* text : invalidTexts) {
            Assert::IsFalse(tryParseNumber(text, number));
        }
        Assert::AreEqual(7, number);
    }

    TEST_METHOD(AcceptsValidPlayerNames) {
        Assert::IsTrue(isValidPlayerName("Ana"));
        Assert::IsTrue(isValidPlayerName("Player_1"));
        Assert::IsTrue(isValidPlayerName("abcdefghij"));
    }

    TEST_METHOD(RejectsInvalidPlayerNames) {
        Assert::IsFalse(isValidPlayerName(""));
        Assert::IsFalse(isValidPlayerName("abcdefghijk"));
        Assert::IsFalse(isValidPlayerName("two words"));
        Assert::IsFalse(isValidPlayerName("bad!"));
    }
    };

    TEST_CLASS(MazeGeneratorTests) {
public:
    TEST_METHOD(EveryMazeIsCorrect) {
        for (int size = MIN_MAZE_SIZE; size <= MAX_MAZE_SIZE; size++) {
            for (int level = 0; level < 3; level++) {
                GameState state;
                startNewGame(state, size, GameMode::Classic, static_cast<Difficulty>(level));

                Assert::IsTrue(state.maze.tiles[0][0] != TileType::Wall, L"start is open");
                Assert::IsTrue(state.maze.tiles[size - 1][size - 1] != TileType::Wall, L"exit is open");
                Assert::IsTrue(isExitReachableWithoutDoors(state.maze), L"exit reachable without doors");
                Assert::IsTrue(countDoors(state.maze) >= 1, L"at least one door");
                Assert::IsTrue(findShortestPathLength(state.maze) >= 2 * (size - 1), L"shortest path length");
                endGame(state);
            }
        }
    }

    TEST_METHOD(AllocatedMazeStartsAsWalls) {
        Maze maze;
        allocateMaze(maze, 12);
        Assert::AreEqual(12, maze.size);
        Assert::IsTrue(maze.tiles[5][7] == TileType::Wall);
        freeMaze(maze);
        Assert::IsTrue(maze.tiles == nullptr);
        Assert::AreEqual(0, maze.size);
    }

    TEST_METHOD(InsideMazeCheck) {
        Maze maze;
        allocateMaze(maze, 10);
        Assert::IsTrue(isInsideMaze(maze, 0, 0));
        Assert::IsTrue(isInsideMaze(maze, 9, 9));
        Assert::IsFalse(isInsideMaze(maze, -1, 0));
        Assert::IsFalse(isInsideMaze(maze, 0, 10));
        freeMaze(maze);
    }
    };

    TEST_CLASS(DifficultyTests) {
public:
    TEST_METHOD(EasyHasMoreDoorsThanHard) {
        GameState easy;
        GameState hard;
        startNewGame(easy, 40, GameMode::Classic, Difficulty::Easy);
        startNewGame(hard, 40, GameMode::Classic, Difficulty::Hard);
        Assert::IsTrue(countDoors(easy.maze) > countDoors(hard.maze));
        endGame(easy);
        endGame(hard);
    }

    TEST_METHOD(EasySeesFartherInFog) {
        GameState state;
        prepareEmptyGame(state, 30, GameMode::Fog);
        state.player.row = 10;
        state.player.col = 10;
        state.difficulty = Difficulty::Easy;
        Assert::IsTrue(isTileVisible(state, 10, 15));
        state.difficulty = Difficulty::Hard;
        Assert::IsFalse(isTileVisible(state, 10, 15));
        endGame(state);
    }

    TEST_METHOD(MoveLimitDependsOnLevel) {
        GameState easy;
        GameState hard;
        startNewGame(easy, 20, GameMode::MoveLimit, Difficulty::Easy);
        startNewGame(hard, 20, GameMode::MoveLimit, Difficulty::Hard);
        int easyShortest = findShortestPathLength(easy.maze);
        int hardShortest = findShortestPathLength(hard.maze);

        Assert::AreEqual(easyShortest * 200 / 100 + 15, easy.moveLimit);
        Assert::AreEqual(hardShortest * 120 / 100 + 5, hard.moveLimit);
        Assert::IsTrue(hard.moveLimit > hardShortest, L"even Hard can be won");
        endGame(easy);
        endGame(hard);
    }
    };

    TEST_CLASS(GameStateTests) {
public:
    TEST_METHOD(NewGameResetsEverything) {
        GameState state;
        startNewGame(state, 12, GameMode::Classic, Difficulty::Normal);
        state.player.row = 3;
        state.moveCount = 99;
        state.tailLength = 4;

        startNewGame(state, 20, GameMode::Fog, Difficulty::Hard);
        Assert::AreEqual(20, state.maze.size);
        Assert::IsTrue(state.mode == GameMode::Fog);
        Assert::IsTrue(state.difficulty == Difficulty::Hard);
        Assert::AreEqual(0, state.player.row);
        Assert::AreEqual(0, state.moveCount);
        Assert::AreEqual(0, state.tailLength);
        endGame(state);
    }

    TEST_METHOD(TimerCountsSeconds) {
        GameState state;
        startNewGame(state, 10, GameMode::Classic, Difficulty::Normal);
        state.startTime = time(nullptr) - 65;
        int seconds = getElapsedSeconds(state);
        Assert::IsTrue(seconds >= 65 && seconds <= 66);
        endGame(state);
    }

    TEST_METHOD(EndGameReleasesMemory) {
        GameState state;
        startNewGame(state, 15, GameMode::Classic, Difficulty::Normal);
        endGame(state);
        Assert::IsTrue(state.maze.tiles == nullptr);
    }
    };

    TEST_CLASS(MovementTests) {
public:
    TEST_METHOD_INITIALIZE(BuildCorridor) {
        prepareEmptyGame(state, 10);
        for (int col = 0; col < 10; col++) {
            state.maze.tiles[0][col] = TileType::Path;
        }
        state.maze.tiles[0][3] = TileType::Door;
    }

    TEST_METHOD_CLEANUP(FreeCorridor) {
        endGame(state);
    }

    TEST_METHOD(CannotLeaveTheMaze) {
        Assert::IsTrue(movePlayer(state, Direction::Up) == MoveResult::Blocked);
        Assert::IsTrue(movePlayer(state, Direction::Left) == MoveResult::Blocked);
    }

    TEST_METHOD(WallsBlockAndChangeNothing) {
        Assert::IsTrue(movePlayer(state, Direction::Down) == MoveResult::Blocked);
        Assert::AreEqual(0, state.moveCount);
        Assert::AreEqual(0, state.player.row);
    }

    TEST_METHOD(WalksOnPath) {
        Assert::IsTrue(movePlayer(state, Direction::Right) == MoveResult::Moved);
        Assert::AreEqual(1, state.player.col);
        Assert::AreEqual(1, state.moveCount);
    }

    TEST_METHOD(PassesThroughDoor) {
        movePlayer(state, Direction::Right);
        movePlayer(state, Direction::Right);
        Assert::IsTrue(movePlayer(state, Direction::Right) == MoveResult::UsedDoor);
        Assert::AreEqual(3, state.player.col);
    }

    TEST_METHOD(TailFollowsHead) {
        movePlayer(state, Direction::Right);
        Assert::AreEqual(1, state.tailLength);
        Assert::AreEqual(0, state.tail[0].col);
        movePlayer(state, Direction::Right);
        movePlayer(state, Direction::Right);
        Assert::AreEqual(2, state.tail[0].col);
        Assert::AreEqual(1, state.tail[1].col);
        Assert::AreEqual(0, state.tail[2].col);
    }

    TEST_METHOD(TailHasMaximumLength) {
        for (int i = 0; i < 9; i++) {
            movePlayer(state, Direction::Right);
        }
        Assert::AreEqual(SNAKE_MAX_TAIL, state.tailLength);
    }

private:
    GameState state;
    };

    TEST_CLASS(WinAndLoseTests) {
public:
    TEST_METHOD(WinOnlyOnTheExit) {
        GameState state;
        prepareEmptyGame(state, 10);
        Assert::IsFalse(hasPlayerWon(state));
        state.player.row = 9;
        state.player.col = 8;
        Assert::IsFalse(hasPlayerWon(state));
        state.player.col = 9;
        Assert::IsTrue(hasPlayerWon(state));
        endGame(state);
    }

    TEST_METHOD(MoveLimitLoss) {
        GameState state;
        startNewGame(state, 15, GameMode::MoveLimit, Difficulty::Normal);
        Assert::IsFalse(hasPlayerLost(state));
        Assert::AreEqual(state.moveLimit, getMovesLeft(state));
        state.moveCount = state.moveLimit - 1;
        Assert::IsFalse(hasPlayerLost(state));
        state.moveCount = state.moveLimit;
        Assert::IsTrue(hasPlayerLost(state));
        endGame(state);
    }

    TEST_METHOD(LastMoveOnExitIsWin) {
        GameState state;
        startNewGame(state, 15, GameMode::MoveLimit, Difficulty::Normal);
        state.moveCount = state.moveLimit;
        state.player.row = 14;
        state.player.col = 14;
        Assert::IsFalse(hasPlayerLost(state));
        Assert::IsTrue(hasPlayerWon(state));
        endGame(state);
    }

    TEST_METHOD(ClassicNeverLoses) {
        GameState state;
        startNewGame(state, 15, GameMode::Classic, Difficulty::Normal);
        state.moveCount = 100000;
        Assert::IsFalse(hasPlayerLost(state));
        endGame(state);
    }
    };

    TEST_CLASS(FogTests) {
public:
    TEST_METHOD(OnlyNearTilesAreVisible) {
        GameState state;
        prepareEmptyGame(state, 20, GameMode::Fog);   // Normal level: radius 4
        state.player.row = 5;
        state.player.col = 5;
        Assert::IsTrue(isTileVisible(state, 5, 9));
        Assert::IsFalse(isTileVisible(state, 5, 10));
        Assert::IsFalse(isTileVisible(state, 15, 15));
        endGame(state);
    }

    TEST_METHOD(ExitIsAlwaysVisible) {
        GameState state;
        prepareEmptyGame(state, 20, GameMode::Fog);
        Assert::IsTrue(isTileVisible(state, 19, 19));
        endGame(state);
    }

    TEST_METHOD(ClassicSeesEverything) {
        GameState state;
        prepareEmptyGame(state, 20, GameMode::Classic);
        Assert::IsTrue(isTileVisible(state, 15, 15));
        endGame(state);
    }
    };

    TEST_CLASS(ScoreTests) {
public:
    TEST_METHOD(ScoreFormula) {
        Assert::IsTrue(calculateScore(20, Difficulty::Normal, 30) > calculateScore(20, Difficulty::Normal, 90), L"faster is better");
        Assert::IsTrue(calculateScore(20, Difficulty::Hard, 60) > calculateScore(20, Difficulty::Easy, 60), L"harder is better");
        Assert::IsTrue(calculateScore(40, Difficulty::Normal, 60) > calculateScore(10, Difficulty::Normal, 60), L"bigger is better");
    }

    TEST_METHOD(RanksAreCountedPerMode) {
        vector<ScoreEntry> scores;
        Assert::AreEqual(1, addScore(scores, makeEntry("Ana", GameMode::Classic, Difficulty::Normal, 20, 60)));
        Assert::AreEqual(1, addScore(scores, makeEntry("Ben", GameMode::Classic, Difficulty::Normal, 20, 30)));
        Assert::AreEqual(2, addScore(scores, makeEntry("Cleo", GameMode::Classic, Difficulty::Normal, 20, 45)));
        Assert::AreEqual(1, addScore(scores, makeEntry("Dan", GameMode::Fog, Difficulty::Normal, 20, 99)));
    }

    TEST_METHOD(TopListIsSortedAndLimited) {
        vector<ScoreEntry> scores;
        addScore(scores, makeEntry("Ana", GameMode::Classic, Difficulty::Normal, 20, 60));
        addScore(scores, makeEntry("Ben", GameMode::Classic, Difficulty::Normal, 20, 30));
        addScore(scores, makeEntry("Cleo", GameMode::Classic, Difficulty::Normal, 20, 45));

        vector<ScoreEntry> top = getTopScores(scores, GameMode::Classic, 10);
        Assert::AreEqual(3, static_cast<int>(top.size()));
        Assert::IsTrue(top[0].name == "Ben" && top[1].name == "Cleo" && top[2].name == "Ana");
        Assert::AreEqual(2, static_cast<int>(getTopScores(scores, GameMode::Classic, 2).size()));
        Assert::IsTrue(getTopScores(scores, GameMode::MoveLimit, 10).empty());
    }

    TEST_METHOD(BestTimeIsPerSetup) {
        vector<ScoreEntry> scores;
        Assert::AreEqual(-1, findBestTime(scores, GameMode::Classic, Difficulty::Normal, 20));
        addScore(scores, makeEntry("Ana", GameMode::Classic, Difficulty::Normal, 20, 60));
        addScore(scores, makeEntry("Ben", GameMode::Classic, Difficulty::Normal, 20, 30));
        Assert::AreEqual(30, findBestTime(scores, GameMode::Classic, Difficulty::Normal, 20));
        Assert::AreEqual(-1, findBestTime(scores, GameMode::Classic, Difficulty::Hard, 20));
        Assert::AreEqual(-1, findBestTime(scores, GameMode::Classic, Difficulty::Normal, 25));
    }

    TEST_METHOD(NewRecordCheck) {
        Assert::IsTrue(isNewRecord(-1, 100));
        Assert::IsTrue(isNewRecord(50, 49));
        Assert::IsFalse(isNewRecord(50, 50));
        Assert::IsFalse(isNewRecord(50, 70));
    }

    TEST_METHOD(SaveAndLoad) {
        vector<ScoreEntry> scores;
        addScore(scores, makeEntry("Ana", GameMode::Classic, Difficulty::Easy, 20, 60));
        addScore(scores, makeEntry("Dan", GameMode::Fog, Difficulty::Hard, 35, 99));

        const string file = "unittest_scores.tmp";
        saveScores(scores, file);
        vector<ScoreEntry> loaded = loadScores(file);
        remove(file.c_str());

        Assert::AreEqual(2, static_cast<int>(loaded.size()));
        Assert::IsTrue(loaded[1].name == "Dan");
        Assert::IsTrue(loaded[1].mode == GameMode::Fog);
        Assert::IsTrue(loaded[1].difficulty == Difficulty::Hard);
        Assert::AreEqual(35, loaded[1].size);
        Assert::AreEqual(99, loaded[1].seconds);
        Assert::AreEqual(scores[1].score, loaded[1].score);
    }

    TEST_METHOD(MissingFileGivesEmptyList) {
        Assert::IsTrue(loadScores("file_that_does_not_exist.tmp").empty());
    }
    };

}  // namespace UnitTest