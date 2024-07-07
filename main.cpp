#include <ncurses.h>
#include <vector>
#include <string>

std::vector<std::string> textBuffer;
int cursorX = 0, cursorY = 0;

void displayText() {
    clear();  // Clear the screen
    for (size_t i = 0; i < textBuffer.size(); i++) {
        mvprintw(i, 0, "%s", textBuffer[i].c_str());  // Print each line
    }
    move(cursorY, cursorX);  // Move the cursor to the current position
    refresh();  // Refresh the screen
}

void handleArrowKeys(int key) {
    switch (key) {
        case KEY_UP:
            if (cursorY > 0) cursorY--;
            break;
        case KEY_DOWN:
            if (cursorY < static_cast<int>(textBuffer.size()) - 1) cursorY++;
            break;
        case KEY_LEFT:
            if (cursorX > 0) cursorX--;
            break;
        case KEY_RIGHT:
            if (cursorX < static_cast<int>(textBuffer[cursorY].size()) - 1) cursorX++;
            break;
        default:
            break;
    }
}


int main() {
    initscr();  // Initialize ncurses
    cbreak();
    noecho();
    keypad(stdscr, TRUE);  // Enable arrow keys

    displayText();

    int ch;
    while ((ch = getch()) != 'q') {  // Press 'q' to quit
        switch (ch) {
            // Handle different key inputs (left, right, up, down, etc.)
        }
    }

    endwin();  // End ncurses mode
    return 0;
}
