# Cacerola Rush - Snake Maze (C++)

A colourful console maze game: guide a snake from the top-left corner to the exit in the bottom-right corner.
3 game modes, 3 levels, a live timer with a time to beat and a saved leaderboard. Always drawn in the center of the window.

## Open and run (Visual Studio 2022)
1. Unzip the folder and double-click `Cacerola-Rush-10.sln`.
2. Choose **Debug | x64** in the toolbar.
3. Press **Ctrl + F5**. Layer 1 is the start-up project, so the game starts.
4. Tests: **Test → Run All Tests** (or open **Test → Test Explorer**).

Needed in the Visual Studio Installer: the workload **Desktop development with C++**
(it contains the "C++ unit test" component used by the UnitTest project).
If Visual Studio asks to "retarget" the projects, click **OK**.

## Solution structure (three layers)
| Project | Layer | Type | Contents | References |
|---|---|---|---|---|
| **Layer 1** (start-up) | Presentation | Application (.exe) | `Cacerola-Rush-10.cpp` (main), `presentation/` | Layer 2, Layer 3 |
| **Layer 2** | Logic | Static library (.lib) | `logic/` - no `cin` / `cout` | Layer 3 |
| **Layer 3** | Data | Static library (.lib) | `data/` - types, dynamic maze array, leaderboard file | - |
| **UnitTest** | Tests | Native unit test (.dll) | `pch.h`, `pch.cpp`, `UnitTest.cpp` | Layer 2, Layer 3 |

A layer only uses the layers below it: `Layer 1 -> Layer 2 -> Layer 3`.
`Common.props` holds the settings shared by all projects (include folders, C++17, `/utf-8`).

| Layer | Files |
|---|---|
| Data | `Types.h`, `Maze`, `Leaderboard` |
| Logic | `MazeGenerator`, `PlayerMovement`, `GameRules`, `Scores`, `InputValidation` |
| Presentation | `Console`, `InputHandler`, `Panel`, `Menu`, `Screens`, `MazeDisplay`, `Game` |

## Controls
Arrow keys or W A S D: move / choose. Enter: select. Q: back. In the leaderboard: Left / Right switches the mode.

## Modes and levels
| Mode | Challenge |
|---|---|
| Classic | no limits - beat the time |
| Dark Maze | fog: you only see the tiles near the snake |
| Move Limit | a limited number of moves |

| Level | Doors (shortcuts) | Fog view | Move limit |
|---|---|---|---|
| Easy | many | 6 tiles | 2 x best route + 15 |
| Normal | some | 4 tiles | 1.5 x best route + 10 |
| Hard | very few | 2 tiles | 1.2 x best route + 5 |

The leaderboard is saved in `leaderboard.txt` in the `Layer 1` folder (the working folder when started from Visual Studio).

Naming: types `PascalCase`, functions and variables `camelCase`, constants `UPPER_SNAKE_CASE`.
