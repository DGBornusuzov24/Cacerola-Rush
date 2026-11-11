#include "presentation/Menu.h"

#include <iostream>
#include <vector>

#include "logic/GameRules.h"
#include "logic/InputValidation.h"
#include "presentation/Console.h"
#include "presentation/InputHandler.h"
#include "presentation/MazeDisplay.h"
#include "presentation/Panel.h"

namespace {

string menuItem(const string& text, bool isSelected) {
    if (isSelected) {
        return panelLine(fgColor(231) + TEXT_BOLD + " ►  " + text, THEME_SELECTED);
    }
    return panelLine(textStyle() + "    " + text);
}

// Returns the chosen index. Q chooses the last item (Exit / Back).
int chooseFromMenu(const string& heading, const vector<string>& items, const vector<string>& descriptions) {
    int count = static_cast<int>(items.size());
    int selected = 0;

    while (true) {
        vector<string> lines = buildTitle();
        lines.insert(lines.end(), {panelTop(), panelTitle(heading), panelSeparator()});
        for (int i = 0; i < count; i++) {
            lines.push_back(menuItem(items[i], i == selected));
        }
        lines.insert(lines.end(), {panelBottom(), " ", fgColor(THEME_TEXT) + descriptions[selected], " ",
                                   fgColor(THEME_DIM) + "↑ ↓  choose      Enter  select      Q  back"});
        drawCentered(lines);

        Key key = readKey();
        if (key == Key::Up) {
            selected = (selected + count - 1) % count;
        } else if (key == Key::Down) {
            selected = (selected + 1) % count;
        } else if (key == Key::Enter) {
            return selected;
        } else if (key == Key::Quit) {
            return count - 1;
        }
    }
}

// A small box where the player types one line. Returns false if the input is closed.
bool askLine(const string& heading, const string& instruction, const string& error, string& text) {
    const string prompt = "  > ";
    int top = drawCentered({panelTop(), panelTitle(heading), panelSeparator(), panelText(instruction), panelLine(""),
                            panelLine(textStyle() + prompt), panelBottom(), " ", error});

    // the cursor goes right after the prompt, inside the box
    cout << moveTo(top + 5, centeredColumn(PANEL_WIDTH + 2) + 1 + visibleLength(prompt))
         << bgColor(THEME_PANEL) << fgColor(THEME_GOLD);
    return readTextLine(text);
}

}  // namespace

int showMainMenu() {
    return chooseFromMenu("MAIN MENU", {"PLAY", "LEADERBOARD", "HOW TO PLAY", "EXIT"},
                          {"Guide the snake out of a random maze.", "See the best players and their stats.",
                           "Controls, symbols, modes and levels.", "Close the game."});
}

int chooseGameMode() {
    int choice = chooseFromMenu("CHOOSE A MODE",
                                {getModeName(GameMode::Classic), getModeName(GameMode::Fog),
                                 getModeName(GameMode::MoveLimit), "Back"},
                                {"No limits. Find the exit as fast as you can.", "Fog! You only see the tiles near the snake.",
                                 "You only have a few moves. Plan your route!", "Return to the main menu."});
    return choice == 3 ? -1 : choice;
}

int chooseDifficulty() {
    int choice = chooseFromMenu("CHOOSE A LEVEL", {"Easy", "Normal", "Hard", "Back"},
                                {"Many shortcut doors, a wide view, a generous move limit.", "A balanced challenge.",
                                 "Few doors, thick fog and a tight move limit.", "Return to the main menu."});
    return choice == 3 ? -1 : choice;
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
        panelTitle("HOW TO PLAY"),
        panelSeparator(),
        panelText("Lead the snake from the top-left corner to the"),
        panelText("exit in the bottom-right corner. Be fast: the timer"),
        panelText("runs, and your time goes to the leaderboard!"),
        panelLine(""),
        panelText("Arrows / W A S D   move the snake"),
        panelText("Enter              select in menus"),
        panelText("Q                   back"),
        panelLine(""),
        panelGlyphText(getSnakeHeadGlyph(), "the snake (it grows while it walks)"),
        panelGlyphText(getWallGlyph(GameMode::Classic, 0, 10), "wall - you cannot pass"),
        panelGlyphText(getDoorGlyph(), "special door - you CAN pass!"),
        panelGlyphText(getExitGlyph(true), "the exit"),
        panelLine(""),
        panelText("Classic     no limits"),
        panelText("Dark Maze   fog: you see only nearby tiles"),
        panelText("Move Limit  you have a limited number of moves"),
        panelText("Easy / Normal / Hard changes doors, fog and limit"),
        panelLine(""),
        panelText("Tip: big mazes scroll - maximize the window!")
    });
}

void showGoodbye() {
    vector<string> lines = buildTitle();
    lines.insert(lines.end(), {fgColor(THEME_TEXT) + "Thanks for playing. See you soon!", " ",
                               fgColor(THEME_DIM) + "Press any key to exit"});
    drawCentered(lines);
    readKey();
}
