#include <ncurses.h>
#include <vector>
#include <string>
#include <fstream>
#include <algorithm>  // For std::find

std::vector<std::string> textBuffer;
int cursorX = 0, cursorY = 0;
bool insertMode = false;

void displayText(int startLine, int endLine) {
    clear();  // Clear the screen
    int maxY, maxX;
    getmaxyx(stdscr, maxY, maxX);  // Get dimensions of the screen

    // Display text within visible window
    for (int i = startLine; i < std::min(static_cast<int>(textBuffer.size()), endLine); i++) {
        mvprintw(i - startLine, 0, "%s", textBuffer[i].c_str());  // Print each line
    }

    // Display status bar
    mvprintw(maxY - 1, 0, "Cursor: (%d, %d) | Press 'q' to quit, 's' to save, 'l' to load, 'f' to find, 'r' to replace",
             cursorY, cursorX);

    // Move cursor to current position relative to visible window
    move(cursorY - startLine, cursorX);

    refresh();  // Refresh the screen
}

void handleArrowKeys(int key, int& startLine, int& endLine) {
    switch (key) {
        case KEY_UP:
            if (cursorY > 0) {
                cursorY--;
                if (cursorY < startLine) {
                    startLine--;
                    endLine--;
                }
            }
            break;
        case KEY_DOWN:
            if (cursorY < static_cast<int>(textBuffer.size()) - 1) {
                cursorY++;
                if (cursorY >= endLine) {
                    startLine++;
                    endLine++;
                }
            }
            break;
        case KEY_LEFT:
            if (cursorX > 0) cursorX--;
            break;
        case KEY_RIGHT:
            if (cursorX < static_cast<int>(textBuffer[cursorY].size())) cursorX++;
            break;
    }
}

void insertCharacter(char ch) {
    if (insertMode) {
        textBuffer[cursorY].insert(cursorX, 1, ch);
        cursorX++;
    } else {
        textBuffer[cursorY][cursorX] = ch;
        if (cursorX < static_cast<int>(textBuffer[cursorY].size() - 1)) {
            cursorX++;
        }
    }
}

void deleteCharacter() {
    if (cursorX > 0) {
        textBuffer[cursorY].erase(cursorX - 1, 1);
        cursorX--;
    }
}

void saveToFile(const std::string& filename) {
    std::ofstream file(filename);
    if (file.is_open()) {
        for (const auto& line : textBuffer) {
            file << line << "\n";
        }
        file.close();
    }
}

void loadFromFile(const std::string& filename) {
    std::ifstream file(filename);
    if (file.is_open()) {
        textBuffer.clear();
        std::string line;
        while (std::getline(file, line)) {
            textBuffer.push_back(line);
        }
        file.close();
    }
}

void search(const std::string& query, int& startLine, int& endLine) {
    for (size_t i = cursorY; i < textBuffer.size(); i++) {
        size_t pos = textBuffer[i].find(query, (i == cursorY ? cursorX : 0));
        if (pos != std::string::npos) {
            cursorY = i;
            cursorX = static_cast<int>(pos);
            return;
        }
    }
    // If not found from current position, wrap around to the beginning
    for (size_t i = 0; i < cursorY; i++) {
        size_t pos = textBuffer[i].find(query);
        if (pos != std::string::npos) {
            cursorY = i;
            cursorX = static_cast<int>(pos);
            return;
        }
    }
}

void replace(const std::string& target, const std::string& replacement) {
    for (auto& line : textBuffer) {
        size_t pos = line.find(target);
        while (pos != std::string::npos) {
            line.replace(pos, target.length(), replacement);
            pos = line.find(target, pos + replacement.length());
        }
    }
}

void undo() {
    // Implement undo functionality if desired
    // Requires keeping a history of edits
}

void redo() {
    // Implement redo functionality if desired
}

int main() {
    initscr();  // Initialize ncurses
    cbreak();
    noecho();
    keypad(stdscr, TRUE);  // Enable arrow keys

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
                saveToFile("textfile.txt");
                break;
            case 'l':  // Load from file
                loadFromFile("textfile.txt");
                cursorX = 0;
                cursorY = 0;
                startLine = 0;
                endLine = LINES - 2;
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
                    echo();
                    replace(targetString, replacementString);
                }
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
