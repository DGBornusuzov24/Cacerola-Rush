#pragma once

#include "data/Types.h"

enum class MoveResult { Moved, UsedDoor, Blocked };

// Moves the snake one tile. Walls block it, doors let it through, the tail follows the head.
MoveResult movePlayer(GameState& state, Direction direction);
