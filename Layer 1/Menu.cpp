#include "Menu.h"

#include <iostream>
#include <vector>

#include "GameRules.h"
#include "InputValidation.h"
#include "Console.h"
#include "InputHandler.h"
#include "MazeDisplay.h"
#include "Panel.h"

static string menuItem(const string& text, bool isSelected) {
    if (isSelected) {
        return boxLine(fgColor(231) + TEXT_BOLD + " ►  " + text, THEME_SELECTED);
    }
    return boxLine(textStyle() + "    " + text);
}

// Shows a menu and returns the chosen item. Q chooses the last item (Exit / Back).
static int chooseFromMenu(const string& heading, const vector<string>& items, const vector<string>& descriptions) {
    int count = (int)items.size();
    int selected = 0;

    while (true) {
        vector<string> lines = buildTitle();
        lines.push_back(boxTop());
        lines.push_back(boxTitle(heading));
        lines.push_back(boxSeparator());
        for (int i = 0; i < count; i++) {
            lines.push_back(menuItem(items[i], i == selected));
        }
        lines.push_back(boxBottom());
        lines.push_back(" ");
        lines.push_back(fgColor(THEME_TEXT) + descriptions[selected]);
        lines.push_back(" ");
        lines.push_back(fgColor(THEME_DIM) + "↑ ↓  choose      Enter  select      Q  back");
        drawCentered(lines);

        Key key = readKey();
        if (key == Key::Up) {
            selected--;
            if (selected < 0) {
                selected = count - 1;   // from the first item jump to the last one
            }
        }
        else if (key == Key::Down) {
            selected++;
            if (selected == count) {
                selected = 0;           // from the last item jump to the first one
            }
        }
        else if (key == Key::Enter) {
            return selected;
        }
        else if (key == Key::Quit) {
            return count - 1;
        }
    }
}

// A small box where the player types one line. Returns false if the input is closed.
static bool askLine(const string& heading, const string& instruction, const string& error, string& text) {
    const string prompt = "  > ";
    vector<string> lines;
    lines.push_back(boxTop());
    lines.push_back(boxTitle(heading));
    lines.push_back(boxSeparator());
    lines.push_back(boxText(instruction));
    lines.push_back(boxLine(""));
    lines.push_back(boxLine(textStyle() + prompt));
    lines.push_back(boxBottom());
    lines.push_back(" ");
    lines.push_back(error);
    int top = drawCentered(lines);

    // put the cursor right after the prompt, inside the box
    int column = centeredColumn(BOX_WIDTH + 2) + 1 + visibleLength(prompt);
    cout << moveTo(top + 5, column) << bgColor(THEME_PANEL) << fgColor(THEME_GOLD);
    return readTextLine(text);
}

int showMainMenu() {
    return chooseFromMenu("MAIN MENU", { "PLAY", "LEADERBOARD", "HOW TO PLAY", "EXIT" },
        { "Guide the snake out of a random maze.", "The best players of every mode.",
         "Controls, symbols and game modes.", "Close the game." });
}

int chooseGameMode() {
    vector<string> items;
    for (int i = 0; i < MODE_COUNT; i++) {
        items.push_back(getModeName((GameMode)i));
    }
    items.push_back("Back");

    int choice = chooseFromMenu("CHOOSE A MODE", items,
        { "No limits. Find the exit as fast as you can.",
         "Fog! You only see the tiles near the snake.",
         "Only a few moves. Plan your route!",
         "Collect keys to open the locked gates.",
         "Fog, keys and a move limit - all at once!",
         "Return to the main menu." });
    if (choice == MODE_COUNT) {
        return -1;
    }
    return choice;
}

int chooseDifficulty(GameMode mode) {
    if (mode == GameMode::Chaos) {
        int choice = chooseFromMenu("HOW MUCH CHAOS?", { "Normal", "Full Chaos", "Back" },
            { "2 keys, normal fog and a fair move limit.",
             "3 keys, thick fog, and the controls flip every 20 moves!",
             "Return to the main menu." });
        if (choice == 0) {
            return (int)Difficulty::Normal;
        }
        if (choice == 1) {
            return (int)Difficulty::FullChaos;
        }
        return -1;
    }

    int choice = chooseFromMenu("CHOOSE A LEVEL", { "Easy", "Normal", "Hard", "Back" },
        { "Many doors, wide view, 1 key, a big move limit.",
         "A balanced challenge.",
         "Few doors, thick fog, 3 keys, a tight move limit.",
         "Return to the main menu." });
    if (choice == 3) {
        return -1;
    }
    return choice;
}

int askMazeSize() {
    string text;
    string error = " ";
    int size = 0;
    while (askLine("MAZE SIZE", "Type a number from 10 to 50, then press Enter.", error, text)) {
        if (tryParseNumber(text, size) && isValidMazeSize(size)) {
            return size;
        }
        error = fgColor(THEME_BAD) + "Invalid size! Please type a whole number from 10 to 50.";
    }
    return MIN_MAZE_SIZE;
}

string askPlayerName() {
    string name;
    string error = " ";
    while (askLine("YOU ESCAPED!", "Your name for the leaderboard (1-10 characters):", error, name)) {
        if (isValidPlayerName(name)) {
            return name;
        }
        error = fgColor(THEME_BAD) + "Invalid name! Use letters, digits or _ (no spaces).";
    }
    return "Player";
}

void showHelp() {
    showBox({
        boxTitle("HOW TO PLAY"),
        boxSeparator(),
        boxText("Lead the snake to the exit in the bottom-right"),
        boxText("corner. Be fast and use few moves for 3 stars!"),
        boxLine(""),
        boxText("Arrows / W A S D  move      H  hint (+5 seconds)"),
        boxText("P  pause                    Q  back"),
        boxLine(""),
        boxGlyphText(getSnakeHeadGlyph(), "the snake"),
        boxGlyphText(getTrailGlyph(), "your trail - where you have been"),
        boxGlyphText(getWallGlyph(GameMode::Classic, 0, 6), "wall - you cannot pass"),
        boxGlyphText(getDoorGlyph(), "secret door - you CAN pass"),
        boxGlyphText(getKeyGlyph(), "key - pick it up"),
        boxGlyphText(getGateGlyph(), "gate - opens with a key"),
        boxGlyphText(getHintGlyph(), "hint - the way to your next goal"),
        boxGlyphText(getExitGlyph(true), "the exit"),
        boxLine(""),
        boxText("Classic     no limits"),
        boxText("Dark Maze   fog: you only see nearby tiles"),
        boxText("Move Limit  a limited number of moves"),
        boxText("Key Hunt    find keys to open the gates"),
        boxText("Chaos       everything at once!")
        });
}

void showGoodbye() {
    vector<string> lines = buildTitle();
    lines.push_back(fgColor(THEME_TEXT) + "Thanks for playing. See you soon!");
    lines.push_back(" ");
    lines.push_back(fgColor(THEME_DIM) + "Press any key to exit");
    drawCentered(lines);
    readKey();
}
