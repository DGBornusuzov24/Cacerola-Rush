# Technical Documentation - Snake Maze

## 1. The game
A snake starts at (0, 0) of a text maze and must reach the exit in the bottom-right corner. The maze is drawn from above
and redrawn after every move (and twice a second, so the timer runs). A new random maze is made for every game.

Rules: choose a mode, a level and a size (10-50). Walls block the snake; special doors (cyan `▒▒`) can be crossed and
are shortcuts. The game is won on the exit `EX`. Unknown keys are ignored, typed sizes and names are validated.

| Mode | Challenge |
|---|---|
| Classic | none - beat your time |
| Dark Maze | `isTileVisible` shows only tiles near the snake (the exit is always visible) |
| Move Limit | limit = shortest route (breadth-first search) * percent + extra; 0 moves left = lost |

| Level | doors = size / | fog radius | limit percent / extra |
|---|---|---|---|
| Easy | 3 | 6 | 200 / 15 |
| Normal | 5 | 4 | 150 / 10 |
| Hard | 10 | 2 | 120 / 5 |

**Timer:** `startTime` is saved when the game starts; `getElapsedSeconds` is the difference to now. `readKey(500)` returns
`Key::None` after half a second without a key, so the screen is redrawn and the timer keeps moving.

**Leaderboard:** after a win the player types a name. `score = size * size * (level + 1) * 1000 / (seconds + 10)`.
Entries are saved in `leaderboard.txt` (`name mode level size seconds moves score`, one game per line). The "time to beat"
is the fastest time of the same mode + level + size (`findBestTime`).

## 2. Architecture - one Visual Studio project per layer
```
Layer 1  (Presentation, .exe, start-up)  --references-->  Layer 2 (Logic, .lib)  --references-->  Layer 3 (Data, .lib)
    \______________________________________references_____________________________________/
UnitTest (.dll)  --references-->  Layer 2, Layer 3
```
**Layer 3 - Data:** `Types.h` (enums and structs), `Maze` (`allocateMaze` / `freeMaze`: the dynamic 2D array
`TileType** tiles`), `Leaderboard` (`loadScores` / `saveScores`).

**Layer 2 - Logic** (no `cin` / `cout`): `MazeGenerator` (recursive backtracker with its own dynamic stack,
`findShortestPathLength`), `PlayerMovement` (`movePlayer`, tail), `GameRules` (start / end, win, lose, fog, timer,
difficulty table), `Scores` (score, rank, top list, best time), `InputValidation` (`tryParseNumber`, `isValidPlayerName`).

**Layer 1 - Presentation:** `Console` (colours, terminal size, `drawCentered`), `InputHandler` (`readKey` with timeout),
`Panel` (boxes, title, names, `formatTime`), `Menu` (arrow-key menus, typed input, help), `Screens` (win, lose, leaderboard),
`MazeDisplay` (`drawGame`, HUD, scrolling view), `Game` (`runApplication`, `playGame`), `Cacerola-Rush-10.cpp` (`main`).

Shared settings are in `Common.props`: include folders of the three layers, C++17, `/utf-8`, `NOMINMAX`.

## 3. Flow
```
main menu -> PLAY -> mode -> level -> size -> game loop (draw, read key with timeout, move)
          -> won: name -> save score -> result screen      lost: "out of moves" screen
          -> LEADERBOARD (Left / Right = mode)   HOW TO PLAY   EXIT
```

## 4. Who did what (fill in)
| Role | Name | Part |
|---|---|---|
| Scrum Trainer | | planning, GitHub board, documentation, presentation |
| Backend Developer | | Layer 2 + Layer 3 |
| Frontend Developer | | Layer 1 |
| QA | | UnitTest project, `docs/QA.md` |

## 5. Presentation outline (about 8 slides)
1. Title and team. 2. Goal and requirements. 3. The game: screenshots of menu, maze, modes, leaderboard.
4. Architecture: the 4 projects and their references. 5. Algorithms: maze generation, BFS, score. 6. Dynamic memory.
7. QA: unit tests in Test Explorer and manual tests. 8. Team work and live demo.

## 6. Note about the example in the task
The "numbers" example in the assignment is one possible variant. This project follows the main requirements
(walls, doors, arrow keys, size 10-50, menu, validation, dynamic memory, three layers).
