#include "logic/PlayerMovement.h"

#include "data/Maze.h"

MoveResult movePlayer(GameState& state, Direction direction) {
    int newRow = state.player.row;
    int newCol = state.player.col;
    switch (direction) {
    case Direction::Up:    newRow--; break;
    case Direction::Down:  newRow++; break;
    case Direction::Left:  newCol--; break;
    case Direction::Right: newCol++; break;
    }

    if (!isInsideMaze(state.maze, newRow, newCol) || state.maze.tiles[newRow][newCol] == TileType::Wall) {
        return MoveResult::Blocked;
    }

    for (int i = SNAKE_MAX_TAIL - 1; i > 0; i--) {   // every tail segment takes the place of the one before it
        state.tail[i] = state.tail[i - 1];
    }
    state.tail[0] = state.player;
    if (state.tailLength < SNAKE_MAX_TAIL) {
        state.tailLength++;
    }

    state.player.row = newRow;
    state.player.col = newCol;
    state.moveCount++;
    return state.maze.tiles[newRow][newCol] == TileType::Door ? MoveResult::UsedDoor : MoveResult::Moved;
}
