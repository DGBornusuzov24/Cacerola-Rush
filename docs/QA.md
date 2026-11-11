# QA Documentation - Snake Maze

## 1. Automated tests - project `UnitTest`
The tests use the **Microsoft Native C++ Unit Test Framework** (`CppUnitTest.h`). The project references Layer 2 (logic)
and Layer 3 (data), so it tests the game rules without the screen or the keyboard.

**Run:** Visual Studio → **Test → Run All Tests**. Results appear in **Test Explorer** (green = passed, red = failed).
A fixed random seed (`TEST_MODULE_INITIALIZE`) makes the results the same on every run.

| Test class | Test methods | What is checked |
|---|---|---|
| `InputValidationTests` | `MazeSizeLimits`, `ParsesValidNumbers`, `RejectsInvalidNumbers`, `AcceptsValidPlayerNames`, `RejectsInvalidPlayerNames` | size 10-50; "15", " 20 " accepted; "", "abc", "12a", "-5", "1.5", very long rejected; names 1-10 characters, letters / digits / `_` |
| `MazeGeneratorTests` | `EveryMazeIsCorrect`, `AllocatedMazeStartsAsWalls`, `InsideMazeCheck` | all sizes 10-50 on all levels: start and exit open, exit reachable without doors, at least one door, shortest path length; dynamic array created and freed |
| `DifficultyTests` | `EasyHasMoreDoorsThanHard`, `EasySeesFartherInFog`, `MoveLimitDependsOnLevel` | the level changes doors, fog radius and move limit; Hard is still winnable |
| `GameStateTests` | `NewGameResetsEverything`, `TimerCountsSeconds`, `EndGameReleasesMemory` | new game resets position, counters and tail; timer; memory released |
| `MovementTests` | `CannotLeaveTheMaze`, `WallsBlockAndChangeNothing`, `WalksOnPath`, `PassesThroughDoor`, `TailFollowsHead`, `TailHasMaximumLength` | movement rules and the snake tail (uses `TEST_METHOD_INITIALIZE` / `TEST_METHOD_CLEANUP` to build and free a test corridor) |
| `WinAndLoseTests` | `WinOnlyOnTheExit`, `MoveLimitLoss`, `LastMoveOnExitIsWin`, `ClassicNeverLoses` | win and lose conditions |
| `FogTests` | `OnlyNearTilesAreVisible`, `ExitIsAlwaysVisible`, `ClassicSeesEverything` | Dark Maze visibility |
| `ScoreTests` | `ScoreFormula`, `RanksAreCountedPerMode`, `TopListIsSortedAndLimited`, `BestTimeIsPerSetup`, `NewRecordCheck`, `SaveAndLoad`, `MissingFileGivesEmptyList` | score, ranks, top 10, time to beat, saving and loading the leaderboard |

**Total: 8 test classes, 34 test methods.**

## 2. Manual test cases
| # | Steps | Expected result |
|---|---|---|
| M1 | Start the program, resize the window | Everything is centered (after the next redraw) |
| M2 | Menus: Up / Down, W / S, Enter, Q, other keys | Highlight moves and wraps; Q = Back / Exit; other keys do nothing |
| M3 | Play → mode → level → size `9`, `51`, `x`, `-3`, `10.5`, empty | Red error message, asks again |
| M4 | Size 10 and 50 | Maze is drawn correctly; size 50 scrolls with the snake in a small window |
| M5 | Move with arrows / WASD | Snake moves, tail grows to 6, counters update |
| M6 | Wait without pressing keys | TIME keeps counting; the exit blinks |
| M7 | Walk into a wall / through a door | Wall message / door message, snake passes the door |
| M8 | Dark Maze Easy vs Hard | Hard shows a much smaller view; the exit is always visible |
| M9 | Move Limit | LEFT counts down (red at 10 or less); at 0 the "Out of moves" screen appears |
| M10 | Win a game | Name prompt: `bad name!`, empty, 11 characters → error; `Zoe` → accepted |
| M11 | After the win | Result screen with time, moves, score, rank; "NEW TIME RECORD" for the first time of a setup |
| M12 | Play the same setup again | BEST shows the old time |
| M13 | LEADERBOARD, Left / Right | Top 10 of each mode, best score first; "No scores yet" for an empty mode |
| M14 | Close and restart the game | The leaderboard is still there (`leaderboard.txt`) |
| M15 | Q in the game, then EXIT | Back to the menu / goodbye screen |
