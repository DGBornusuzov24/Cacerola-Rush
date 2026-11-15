#include "PlayerMovement.h"

#include "Maze.h"

bool areControlsReversed(const GameState& state) {
    if (state.difficulty != Difficulty::FullChaos) {
        return false;
    }
    int round = state.moveCount / CHAOS_FLIP_MOVES;
    return round % 2 == 1;
}

static Direction getOpposite(Direction direction) {
    if (direction == Direction::Up) {
        return Direction::Down;
    }
    if (direction == Direction::Down) {
        return Direction::Up;
    }
    if (direction == Direction::Left) {
        return Direction::Right;
    }
    return Direction::Left;
}

MoveResult movePlayer(GameState& state, Direction direction) {
    if (areControlsReversed(state)) {
        direction = getOpposite(direction);
    }

    Position target = getNeighbour(state.player, direction);
    if (!isInsideMaze(state.maze, target) || getTile(state.maze, target) == TileType::Wall) {
        return MoveResult::Blocked;
    }
    TileType tile = getTile(state.maze, target);
    if (tile == TileType::Gate && state.keysHeld == 0) {
        return MoveResult::Locked;
    }

    // The tail follows the head: the old head position becomes the first tail part.
    state.tail.insert(state.tail.begin(), state.player);
    if ((int)state.tail.size() > SNAKE_MAX_TAIL) {
        state.tail.pop_back();
    }
    state.player = target;
    state.moveCount++;
    state.maze.visited[target.row][target.col] = true;

    if (tile == TileType::Key) {
        state.keysCollected++;
        state.keysHeld++;
        setTile(state.maze, target, TileType::Path);
        return MoveResult::PickedKey;
    }
    if (tile == TileType::Gate) {
        state.keysHeld--;
        setTile(state.maze, target, TileType::Path);
        return MoveResult::OpenedGate;
    }
    if (tile == TileType::Door) {
        return MoveResult::UsedDoor;
    }
    return MoveResult::Moved;
}
