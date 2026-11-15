#pragma once

#include <string>
using namespace std;

enum class Key { Up, Down, Left, Right, Enter, Quit, Hint, Pause, Unknown, None };

// Waits for one key: arrows or W/A/S/D, Enter, Q, H, P.
// With a timeout (in ms) it returns Key::None when nothing was pressed - this keeps the timer running.
Key readKey(int timeoutMs = -1);

// Reads one typed line. Returns false if the input is closed.
bool readTextLine(string& text);
