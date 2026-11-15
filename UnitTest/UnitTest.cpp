#include "pch.h"

#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <vector>

#include "Leaderboard.h"
#include "Maze.h"
#include "GameRules.h"
#include "InputValidation.h"
#include "MazeGenerator.h"
#include "PlayerMovement.h"
#include "Scores.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

// ---------- helpers ----------

// A maze made only of walls. Every test opens the tiles it needs.
static void prepareEmptyGame(GameState& state, int size, GameMode mode, Difficulty difficulty) {
    startNewGame(state, MIN_MAZE_SIZE, mode, difficulty);
    allocateMaze(state.maze, size);
    state.player = Position();
    state.tail.clear();
    state.keys.clear();
    state.moveCount = 0;
    state.moveLimit = 0;
}

// A straight corridor in row 0: the snake starts on its left end.
static void prepareCorridor(GameState& state) {
    prepareEmptyGame(state, 10, GameMode::Classic, Difficulty::Normal);
    for (int col = 0; col < 10; col++) {
        state.maze.tiles[0][col] = TileType::Path;
    }
}

static int countTiles(const Maze& maze, TileType type) {
    int count = 0;
    for (int row = 0; row < maze.size; row++) {
        for (int col = 0; col < maze.size; col++) {
            if (maze.tiles[row][col] == type) {
                count++;
            }
        }
    }
    return count;
}

static Direction directionTo(Position from, Position to) {
    if (to.row < from.row) {
        return Direction::Up;
    }
    if (to.row > from.row) {
        return Direction::Down;
    }
    if (to.col < from.col) {
        return Direction::Left;
    }
    return Direction::Right;
}

// Walks the snake along the shortest way to 'goal' like a player would. Returns false if a move fails.
static bool walkTo(GameState& state, Position goal) {
    vector<Position> path = findPath(state.maze, state.player, goal);
    for (int i = 1; i < (int)path.size(); i++) {
        Direction direction = directionTo(state.player, path[i]);
        if (areControlsReversed(state)) {
            direction = directionTo(path[i], state.player);   // the player presses the opposite key
        }
        MoveResult result = movePlayer(state, direction);
        if (result == MoveResult::Blocked || result == MoveResult::Locked) {
            return false;
        }
    }
    return true;
}

// Plays a whole game: every key in order, then the exit.
static bool playWholeGame(GameState& state) {
    for (Position key : state.keys) {
        if (!walkTo(state, key)) {
            return false;
        }
    }
    return walkTo(state, getExitPosition(state.maze));
}

static ScoreEntry makeEntry(const string& name, GameMode mode, Difficulty difficulty, int size, int seconds) {
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

namespace UnitTest {

    TEST_MODULE_INITIALIZE(SetupRandomSeed) {
        srand(12345);   // a fixed seed: the same results on every run
    }

    TEST_CLASS(InputValidationTests) {
public:
    TEST_METHOD(MazeSizeLimits) {
        Assert::IsFalse(isValidMazeSize(9));
        Assert::IsTrue(isValidMazeSize(10));
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

    TEST_METHOD(PlayerNames) {
        Assert::IsTrue(isValidPlayerName("Ana"));
        Assert::IsTrue(isValidPlayerName("Player_1"));
        Assert::IsTrue(isValidPlayerName("abcdefghij"));
        Assert::IsFalse(isValidPlayerName(""));
        Assert::IsFalse(isValidPlayerName("abcdefghijk"));
        Assert::IsFalse(isValidPlayerName("two words"));
        Assert::IsFalse(isValidPlayerName("bad!"));
    }
    };

    TEST_CLASS(MazeTests) {
public:
    TEST_METHOD(AllocatedMazeIsAllWalls) {
        Maze maze;
        allocateMaze(maze, 12);
        Assert::AreEqual(12, maze.size);
        Assert::IsTrue(maze.tiles[5][7] == TileType::Wall);
        Assert::IsFalse(maze.visited[5][7]);
        freeMaze(maze);
        Assert::IsTrue(maze.tiles == nullptr);
        Assert::IsTrue(maze.visited == nullptr);
    }

    TEST_METHOD(InsideAndExit) {
        Maze maze;
        allocateMaze(maze, 10);
        Assert::IsTrue(isInsideMaze(maze, makePosition(0, 0)));
        Assert::IsTrue(isInsideMaze(maze, makePosition(9, 9)));
        Assert::IsFalse(isInsideMaze(maze, makePosition(-1, 0)));
        Assert::IsFalse(isInsideMaze(maze, makePosition(0, 10)));
        Assert::AreEqual(9, getExitPosition(maze).row);
        Assert::AreEqual(9, getExitPosition(maze).col);
        freeMaze(maze);
    }

    TEST_METHOD(PositionHelpers) {
        Position start = makePosition(4, 7);
        Assert::IsTrue(isSamePosition(start, makePosition(4, 7)));
        Assert::IsFalse(isSamePosition(start, makePosition(7, 4)));
        Assert::AreEqual(3, getNeighbour(start, Direction::Up).row);
        Assert::AreEqual(5, getNeighbour(start, Direction::Down).row);
        Assert::AreEqual(6, getNeighbour(start, Direction::Left).col);
        Assert::AreEqual(8, getNeighbour(start, Direction::Right).col);
    }

    TEST_METHOD(EveryMazeCanBeSolved) {
        for (int size = MIN_MAZE_SIZE; size <= MAX_MAZE_SIZE; size++) {
            for (int level = 0; level < 3; level++) {
                GameState state;
                startNewGame(state, size, GameMode::Classic, (Difficulty)level);
                Assert::IsTrue(state.maze.tiles[0][0] != TileType::Wall);
                Assert::IsTrue(state.maze.tiles[size - 1][size - 1] != TileType::Wall);
                Assert::IsTrue(countTiles(state.maze, TileType::Door) >= 1, L"at least one door");
                Assert::IsTrue(state.bestRoute >= 2 * (size - 1), L"the route cannot be shorter than a straight line");
                Assert::IsTrue(playWholeGame(state));
                Assert::IsTrue(hasPlayerWon(state));
                endGame(state);
            }
        }
    }
    };

    TEST_CLASS(PathTests) {
public:
    TEST_METHOD(PathGoesFromStartToGoal) {
        GameState state;
        prepareCorridor(state);
        vector<Position> path = findPath(state.maze, makePosition(0, 0), makePosition(0, 5));
        Assert::AreEqual(6, (int)path.size());
        Assert::AreEqual(0, path[0].col);
        Assert::AreEqual(5, path[5].col);
        endGame(state);
    }

    TEST_METHOD(EveryStepIsANeighbour) {
        GameState state;
        startNewGame(state, 25, GameMode::Classic, Difficulty::Normal);
        vector<Position> path = findPath(state.maze, makePosition(0, 0), getExitPosition(state.maze));
        for (int i = 1; i < (int)path.size(); i++) {
            int distance = abs(path[i].row - path[i - 1].row) + abs(path[i].col - path[i - 1].col);
            Assert::AreEqual(1, distance);
            Assert::IsTrue(state.maze.tiles[path[i].row][path[i].col] != TileType::Wall);
        }
        endGame(state);
    }

    TEST_METHOD(NoPathThroughWalls) {
        GameState state;
        prepareCorridor(state);
        state.maze.tiles[0][3] = TileType::Wall;
        Assert::IsTrue(findPath(state.maze, makePosition(0, 0), makePosition(0, 5)).empty());
        endGame(state);
    }
    };

    TEST_CLASS(MovementTests) {
public:
    TEST_METHOD_INITIALIZE(BuildCorridor) {
        prepareCorridor(state);
    }

    TEST_METHOD_CLEANUP(FreeCorridor) {
        endGame(state);
    }

    TEST_METHOD(WallsAndBordersBlock) {
        Assert::IsTrue(movePlayer(state, Direction::Up) == MoveResult::Blocked);
        Assert::IsTrue(movePlayer(state, Direction::Left) == MoveResult::Blocked);
        Assert::IsTrue(movePlayer(state, Direction::Down) == MoveResult::Blocked);
        Assert::AreEqual(0, state.moveCount);
    }

    TEST_METHOD(WalksAndLeavesATrail) {
        Assert::IsTrue(movePlayer(state, Direction::Right) == MoveResult::Moved);
        Assert::AreEqual(1, state.player.col);
        Assert::AreEqual(1, state.moveCount);
        Assert::IsTrue(state.maze.visited[0][1]);
        Assert::IsFalse(state.maze.visited[0][2]);
    }

    TEST_METHOD(PassesThroughDoor) {
        state.maze.tiles[0][1] = TileType::Door;
        Assert::IsTrue(movePlayer(state, Direction::Right) == MoveResult::UsedDoor);
        Assert::AreEqual(1, state.player.col);
    }

    TEST_METHOD(TailFollowsTheHead) {
        movePlayer(state, Direction::Right);
        movePlayer(state, Direction::Right);
        movePlayer(state, Direction::Right);
        Assert::AreEqual(3, (int)state.tail.size());
        Assert::AreEqual(2, state.tail[0].col);
        Assert::AreEqual(1, state.tail[1].col);
        Assert::AreEqual(0, state.tail[2].col);
        for (int i = 0; i < 6; i++) {
            movePlayer(state, Direction::Right);
        }
        Assert::AreEqual(SNAKE_MAX_TAIL, (int)state.tail.size());
    }

    TEST_METHOD(KeyIsPickedUp) {
        state.maze.tiles[0][1] = TileType::Key;
        Assert::IsTrue(movePlayer(state, Direction::Right) == MoveResult::PickedKey);
        Assert::AreEqual(1, state.keysCollected);
        Assert::AreEqual(1, state.keysHeld);
        Assert::IsTrue(state.maze.tiles[0][1] == TileType::Path);
    }

    TEST_METHOD(GateWithoutKeyIsLocked) {
        state.maze.tiles[0][1] = TileType::Gate;
        Assert::IsTrue(movePlayer(state, Direction::Right) == MoveResult::Locked);
        Assert::AreEqual(0, state.player.col);
        Assert::AreEqual(0, state.moveCount);
    }

    TEST_METHOD(GateOpensWithKey) {
        state.maze.tiles[0][1] = TileType::Key;
        state.maze.tiles[0][2] = TileType::Gate;
        movePlayer(state, Direction::Right);
        Assert::IsTrue(movePlayer(state, Direction::Right) == MoveResult::OpenedGate);
        Assert::AreEqual(0, state.keysHeld);
        Assert::IsTrue(state.maze.tiles[0][2] == TileType::Path);
    }

private:
    GameState state;
    };

    TEST_CLASS(KeyHuntTests) {
public:
    TEST_METHOD(KeysAndGatesArePlaced) {
        for (int level = 0; level < 3; level++) {
            GameState state;
            startNewGame(state, 21, GameMode::Keys, (Difficulty)level);
            Assert::AreEqual(level + 1, (int)state.keys.size());
            Assert::AreEqual(level + 1, countTiles(state.maze, TileType::Key));
            Assert::AreEqual(level + 1, countTiles(state.maze, TileType::Gate));
            Assert::AreEqual(0, countTiles(state.maze, TileType::Door), L"no doors around the gates");
            endGame(state);
        }
    }

    TEST_METHOD(ExitIsLockedAtTheStart) {
        GameState state;
        startNewGame(state, 20, GameMode::Keys, Difficulty::Normal);
        Assert::IsFalse(walkTo(state, getExitPosition(state.maze)), L"a gate must stop the snake");
        endGame(state);
    }

    TEST_METHOD(EveryKeyHuntCanBeWon) {
        for (int size = MIN_MAZE_SIZE; size <= MAX_MAZE_SIZE; size++) {
            for (int level = 0; level < 3; level++) {
                GameState state;
                startNewGame(state, size, GameMode::Keys, (Difficulty)level);
                Assert::IsTrue(playWholeGame(state));
                Assert::IsTrue(hasPlayerWon(state));
                Assert::AreEqual(state.bestRoute, state.moveCount);
                endGame(state);
            }
        }
    }
    };

    TEST_CLASS(ChaosTests) {
public:
    TEST_METHOD(ChaosHasEveryChallenge) {
        Assert::IsTrue(modeHasFog(GameMode::Chaos));
        Assert::IsTrue(modeHasKeys(GameMode::Chaos));
        Assert::IsTrue(modeHasMoveLimit(GameMode::Chaos));
        Assert::IsFalse(modeHasFog(GameMode::Classic));
        Assert::IsFalse(modeHasKeys(GameMode::MoveLimit));

        GameState state;
        startNewGame(state, 20, GameMode::Chaos, Difficulty::Normal);
        Assert::AreEqual(2, (int)state.keys.size());
        Assert::IsTrue(state.moveLimit > state.bestRoute);
        Assert::IsFalse(isTileVisible(state, 15, 5));
        endGame(state);
    }

    TEST_METHOD(ControlsFlipOnlyInFullChaos) {
        GameState state;
        prepareEmptyGame(state, 10, GameMode::Chaos, Difficulty::FullChaos);
        state.moveCount = 19;
        Assert::IsFalse(areControlsReversed(state));
        state.moveCount = 20;
        Assert::IsTrue(areControlsReversed(state));
        state.moveCount = 40;
        Assert::IsFalse(areControlsReversed(state));
        state.difficulty = Difficulty::Normal;
        state.moveCount = 20;
        Assert::IsFalse(areControlsReversed(state));
        endGame(state);
    }

    TEST_METHOD(ReversedControlsMoveTheOtherWay) {
        GameState state;
        prepareCorridor(state);
        state.mode = GameMode::Chaos;
        state.difficulty = Difficulty::FullChaos;
        state.player = makePosition(0, 5);
        state.moveCount = 20;
        movePlayer(state, Direction::Left);
        Assert::AreEqual(6, state.player.col);
        endGame(state);
    }

    TEST_METHOD(EveryChaosGameCanBeWonInTime) {
        for (int size = MIN_MAZE_SIZE; size <= MAX_MAZE_SIZE; size++) {
            GameState normal;
            GameState full;
            startNewGame(normal, size, GameMode::Chaos, Difficulty::Normal);
            startNewGame(full, size, GameMode::Chaos, Difficulty::FullChaos);
            Assert::IsTrue(playWholeGame(normal) && hasPlayerWon(normal) && !hasPlayerLost(normal));
            Assert::IsTrue(playWholeGame(full) && hasPlayerWon(full) && !hasPlayerLost(full));
            endGame(normal);
            endGame(full);
        }
    }
    };

    TEST_CLASS(LevelTests) {
public:
    TEST_METHOD(EasyHasMoreDoorsThanHard) {
        GameState easy;
        GameState hard;
        startNewGame(easy, 40, GameMode::Classic, Difficulty::Easy);
        startNewGame(hard, 40, GameMode::Classic, Difficulty::Hard);
        Assert::IsTrue(countTiles(easy.maze, TileType::Door) > countTiles(hard.maze, TileType::Door));
        endGame(easy);
        endGame(hard);
    }

    TEST_METHOD(EasySeesFartherInTheFog) {
        GameState state;
        prepareEmptyGame(state, 30, GameMode::Fog, Difficulty::Easy);
        state.player = makePosition(10, 10);
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
        Assert::AreEqual(easy.bestRoute * 200 / 100 + 15, easy.moveLimit);
        Assert::AreEqual(hard.bestRoute * 120 / 100 + 5, hard.moveLimit);
        Assert::AreEqual(0, (int)easy.keys.size(), L"Move Limit has no keys");
        endGame(easy);
        endGame(hard);
    }
    };

    TEST_CLASS(FogTests) {
public:
    TEST_METHOD(OnlyNearTilesAreVisible) {
        GameState state;
        prepareEmptyGame(state, 20, GameMode::Fog, Difficulty::Normal);   // Normal: radius 4
        state.player = makePosition(5, 5);
        Assert::IsTrue(isTileVisible(state, 5, 9));
        Assert::IsFalse(isTileVisible(state, 5, 10));
        Assert::IsTrue(isTileVisible(state, 19, 19), L"the exit is always visible");
        endGame(state);
    }

    TEST_METHOD(KeysAndTrailShineThroughTheFog) {
        GameState state;
        prepareEmptyGame(state, 20, GameMode::Fog, Difficulty::Normal);
        state.maze.tiles[15][3] = TileType::Key;
        state.maze.visited[12][12] = true;
        Assert::IsTrue(isTileVisible(state, 15, 3));
        Assert::IsTrue(isTileVisible(state, 12, 12));
        endGame(state);
    }

    TEST_METHOD(ClassicSeesEverything) {
        GameState state;
        prepareEmptyGame(state, 20, GameMode::Classic, Difficulty::Normal);
        Assert::IsTrue(isTileVisible(state, 15, 15));
        endGame(state);
    }
    };

    TEST_CLASS(GameStateTests) {
public:
    TEST_METHOD(NewGameResetsEverything) {
        GameState state;
        startNewGame(state, 12, GameMode::Keys, Difficulty::Hard);
        state.player = makePosition(3, 3);
        state.moveCount = 99;
        state.hintsUsed = 2;
        startNewGame(state, 20, GameMode::Fog, Difficulty::Easy);
        Assert::AreEqual(20, state.maze.size);
        Assert::IsTrue(state.mode == GameMode::Fog);
        Assert::AreEqual(0, state.player.row);
        Assert::AreEqual(0, state.moveCount);
        Assert::AreEqual(0, state.hintsUsed);
        Assert::AreEqual(0, (int)state.keys.size());
        Assert::IsTrue(state.maze.visited[0][0], L"the start is part of the trail");
        endGame(state);
        Assert::IsTrue(state.maze.tiles == nullptr);
    }

    TEST_METHOD(TimerAndPause) {
        GameState state;
        startNewGame(state, 10, GameMode::Classic, Difficulty::Normal);
        state.startTime = time(nullptr) - 65;
        Assert::IsTrue(getElapsedSeconds(state) >= 65);
        continueAfterPause(state, 60);
        Assert::IsTrue(getElapsedSeconds(state) <= 6, L"the pause does not count");
        restartTimer(state);
        Assert::IsTrue(getElapsedSeconds(state) <= 1);
        endGame(state);
    }

    TEST_METHOD(WinOnlyOnTheExit) {
        GameState state;
        prepareEmptyGame(state, 10, GameMode::Classic, Difficulty::Normal);
        Assert::IsFalse(hasPlayerWon(state));
        state.player = makePosition(9, 9);
        Assert::IsTrue(hasPlayerWon(state));
        endGame(state);
    }

    TEST_METHOD(MoveLimitLoss) {
        GameState state;
        startNewGame(state, 15, GameMode::MoveLimit, Difficulty::Normal);
        Assert::AreEqual(state.moveLimit, getMovesLeft(state));
        state.moveCount = state.moveLimit - 1;
        Assert::IsFalse(hasPlayerLost(state));
        state.moveCount = state.moveLimit;
        Assert::IsTrue(hasPlayerLost(state));
        state.player = getExitPosition(state.maze);
        Assert::IsFalse(hasPlayerLost(state), L"reaching the exit with the last move is a win");
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

    TEST_CLASS(HintTests) {
public:
    TEST_METHOD(HintLeadsToTheNextKey) {
        GameState state;
        startNewGame(state, 20, GameMode::Keys, Difficulty::Normal);
        vector<Position> hint = useHint(state);
        Assert::AreEqual(state.keys[0].row, hint.back().row);
        Assert::AreEqual(state.keys[0].col, hint.back().col);
        endGame(state);
    }

    TEST_METHOD(HintLeadsToTheExitWithoutKeys) {
        GameState state;
        startNewGame(state, 20, GameMode::Classic, Difficulty::Normal);
        vector<Position> hint = useHint(state);
        Assert::AreEqual(19, hint.back().row);
        Assert::AreEqual(19, hint.back().col);
        endGame(state);
    }

    TEST_METHOD(HintsRunOutAndCostTime) {
        GameState state;
        startNewGame(state, 20, GameMode::Classic, Difficulty::Normal);
        for (int i = 0; i < MAX_HINTS; i++) {
            Assert::IsFalse(useHint(state).empty());
        }
        Assert::IsTrue(useHint(state).empty());
        Assert::AreEqual(0, getHintsLeft(state));
        Assert::IsTrue(getElapsedSeconds(state) >= MAX_HINTS * HINT_PENALTY_SECONDS);
        endGame(state);
    }
    };

    TEST_CLASS(ProgressAndStarsTests) {
public:
    TEST_METHOD(ProgressFromZeroToHundred) {
        GameState state;
        startNewGame(state, 15, GameMode::Classic, Difficulty::Normal);
        Assert::AreEqual(0, getProgressPercent(state));
        state.player = getExitPosition(state.maze);
        Assert::AreEqual(100, getProgressPercent(state));
        endGame(state);
    }

    TEST_METHOD(FewerMovesGiveMoreStars) {
        GameState state;
        startNewGame(state, 15, GameMode::Classic, Difficulty::Normal);
        state.moveCount = state.bestRoute;
        Assert::AreEqual(3, getStarRating(state));
        state.moveCount = state.bestRoute * 2;
        Assert::AreEqual(2, getStarRating(state));
        state.moveCount = state.bestRoute * 3;
        Assert::AreEqual(1, getStarRating(state));
        endGame(state);
    }
    };

    TEST_CLASS(ScoreTests) {
public:
    TEST_METHOD(ScoreFormula) {
        Assert::IsTrue(calculateScore(20, Difficulty::Normal, 30) > calculateScore(20, Difficulty::Normal, 90));
        Assert::IsTrue(calculateScore(20, Difficulty::Hard, 60) > calculateScore(20, Difficulty::Easy, 60));
        Assert::IsTrue(calculateScore(20, Difficulty::FullChaos, 60) > calculateScore(20, Difficulty::Hard, 60));
        Assert::IsTrue(calculateScore(40, Difficulty::Normal, 60) > calculateScore(10, Difficulty::Normal, 60));
    }

    TEST_METHOD(RanksAndTopList) {
        vector<ScoreEntry> scores;
        Assert::AreEqual(1, addScore(scores, makeEntry("Ana", GameMode::Classic, Difficulty::Normal, 20, 60)));
        Assert::AreEqual(1, addScore(scores, makeEntry("Ben", GameMode::Classic, Difficulty::Normal, 20, 30)));
        Assert::AreEqual(2, addScore(scores, makeEntry("Cleo", GameMode::Classic, Difficulty::Normal, 20, 45)));
        Assert::AreEqual(1, addScore(scores, makeEntry("Dan", GameMode::Chaos, Difficulty::FullChaos, 20, 99)));

        vector<ScoreEntry> top = getTopScores(scores, GameMode::Classic, 10);
        Assert::AreEqual(3, (int)top.size());
        Assert::IsTrue(top[0].name == "Ben" && top[1].name == "Cleo" && top[2].name == "Ana");
        Assert::AreEqual(2, (int)getTopScores(scores, GameMode::Classic, 2).size());
        Assert::IsTrue(getTopScores(scores, GameMode::Keys, 10).empty());
    }

    TEST_METHOD(TimeToBeat) {
        vector<ScoreEntry> scores;
        Assert::AreEqual(-1, findBestTime(scores, GameMode::Classic, Difficulty::Normal, 20));
        addScore(scores, makeEntry("Ana", GameMode::Classic, Difficulty::Normal, 20, 60));
        addScore(scores, makeEntry("Ben", GameMode::Classic, Difficulty::Normal, 20, 30));
        Assert::AreEqual(30, findBestTime(scores, GameMode::Classic, Difficulty::Normal, 20));
        Assert::AreEqual(-1, findBestTime(scores, GameMode::Classic, Difficulty::Hard, 20));
        Assert::IsTrue(isNewRecord(-1, 100));
        Assert::IsTrue(isNewRecord(50, 49));
        Assert::IsFalse(isNewRecord(50, 50));
    }

    TEST_METHOD(SaveAndLoad) {
        vector<ScoreEntry> scores;
        addScore(scores, makeEntry("Ana", GameMode::Keys, Difficulty::Easy, 20, 60));
        addScore(scores, makeEntry("Dan", GameMode::Chaos, Difficulty::FullChaos, 35, 99));

        const string file = "unittest_scores.tmp";
        saveScores(scores, file);
        vector<ScoreEntry> loaded = loadScores(file);
        remove(file.c_str());

        Assert::AreEqual(2, (int)loaded.size());
        Assert::IsTrue(loaded[0].mode == GameMode::Keys);
        Assert::IsTrue(loaded[1].name == "Dan");
        Assert::IsTrue(loaded[1].mode == GameMode::Chaos);
        Assert::IsTrue(loaded[1].difficulty == Difficulty::FullChaos);
        Assert::AreEqual(99, loaded[1].seconds);
        Assert::AreEqual(scores[1].score, loaded[1].score);
    }

    TEST_METHOD(MissingFileGivesEmptyList) {
        Assert::IsTrue(loadScores("file_that_does_not_exist.tmp").empty());
    }
    };

}  // namespace UnitTest
