#pragma once

#include "Types.h"

enum class MoveResult { Moved, UsedDoor, PickedKey, OpenedGate, Locked, Blocked };

const int CHAOS_FLIP_MOVES = 20;   // Full Chaos: the controls flip every 20 moves

// True in Full Chaos while the controls are flipped (moves 20-39, 60-79, ...).
bool areControlsReversed(const GameState& state);

// Moves the snake one tile. Walls block it, doors let it through, keys are picked up
// and a gate opens only if the snake holds a key.
MoveResult movePlayer(GameState& state, Direction direction);
