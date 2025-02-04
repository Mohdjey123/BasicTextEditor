#include <ncurses.h>
#include "texteditor.h"

int main() {
    initscr();  // Initialize ncurses
    cbreak();
    noecho();
    keypad(stdscr, TRUE);  // Enable arrow keys

    // Initialize text buffer with some default text
    textBuffer = {
        "Welcome to My Text Editor! This is a sample text.",
        "Press 'q' to quit, 's' to save, 'l' to load, 'f' to find, 'r' to replace.",
        "Use arrow keys to navigate. Press 'i' to toggle insert mode."
    };

    int startLine = 0;
    int endLine = LINES - 2;  // Visible lines excluding status bar

    int ch;
    while ((ch = getch()) != 'q') {  // Press 'q' to quit
        switch (ch) {
            case KEY_UP:
            case KEY_DOWN:
            case KEY_LEFT:
            case KEY_RIGHT:
                handleArrowKeys(ch, startLine, endLine);
                break;
            case 'i':  // Toggle insert mode
                insertMode = !insertMode;
                break;
            case 'd':  // Delete mode
                deleteCharacter();
                break;
            case 's':  // Save to file
                {
                    mvprintw(LINES - 1, 0, "Save as (filename): ");
                    char filename[100];
                    echo();
                    getstr(filename);  // Get user input for filename
                    noecho();
                    saveToFile(filename);
                }
                break;
            case 'l':  // Load from file
                {
                    mvprintw(LINES - 1, 0, "Load from (filename): ");
                    char filename[100];
                    echo();
                    getstr(filename);  // Get user input for filename
                    noecho();
                    loadFromFile(filename);
                    cursorX = 0;
                    cursorY = 0;
                    startLine = 0;
                    endLine = LINES - 2;
                }
                break;
            case 'f':  // Find
                {
                    mvprintw(LINES - 1, 0, "Find: ");
                    char findString[100];
                    echo();
                    getstr(findString);  // Get user input for the string to find
                    noecho();
                    search(findString, startLine, endLine);
                }
                break;
            case 'r':  // Replace
                {
                    mvprintw(LINES - 1, 0, "Replace: ");
                    char targetString[100], replacementString[100];
                    echo();
                    getstr(targetString);  // Get user input for target string
                    mvprintw(LINES, 0, "With: ");
                    getstr(replacementString);  // Get user input for replacement string
                    noecho();
                    replace(targetString, replacementString);
                }
                break;
            case 'u':  // Undo
                undo();
                break;
            case 'z':  // Redo
                redo();
                break;
            default:  // Insert characters
                insertCharacter(ch);
                break;
        }

        displayText(startLine, endLine);  // Update display
    }

    endwin();  // End ncurses mode
    return 0;
}
