#pragma once

#include <string>
using namespace std;

enum class Key { Up, Down, Left, Right, Enter, Quit, Unknown, None };

// Call once at the start / end: the keyboard then reacts to every key without waiting for Enter.
void startKeyboard();
void stopKeyboard();

// Waits for one key (arrows or W/A/S/D, Enter, Q). With a timeout (in ms) it returns Key::None when
// nothing was pressed - this keeps the game timer running.
Key readKey(int timeoutMs = -1);

// Reads one typed line. Returns false if the input is closed.
bool readTextLine(string& text);
