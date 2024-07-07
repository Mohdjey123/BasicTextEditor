#include "texteditor.h"
#include <ncurses.h>
#include <fstream>
#include <algorithm>  // For std::find
#include <stack>      // For undo and redo stacks

std::vector<std::string> textBuffer;
int cursorX = 0, cursorY = 0;
bool insertMode = false;

// Undo and redo stacks
std::stack<std::vector<std::string>> undoStack;
std::stack<std::vector<std::string>> redoStack;

void displayText(int startLine, int endLine) {
    clear();  // Clear the screen
    int maxY, maxX;
    getmaxyx(stdscr, maxY, maxX);  // Get dimensions of the screen

    // Display text within visible window
    for (int i = startLine; i < std::min(static_cast<int>(textBuffer.size()), endLine); i++) {
        mvprintw(i - startLine, 0, "%s", textBuffer[i].c_str());  // Print each line
    }

    // Display status bar
    mvprintw(maxY - 1, 0, "Cursor: (%d, %d) | Press 'q' to quit, 's' to save, 'l' to load, 'f' to find, 'r' to replace, 'u' to undo, 'z' to redo",
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
    // Push current state for undo
    undoStack.push(textBuffer);

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
    // Push current state for undo
    undoStack.push(textBuffer);

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
    // Push current state for undo
    undoStack.push(textBuffer);

    for (auto& line : textBuffer) {
        size_t pos = line.find(target);
        while (pos != std::string::npos) {
            line.replace(pos, target.length(), replacement);
            pos = line.find(target, pos + replacement.length());
        }
    }
}

void undo() {
    if (!undoStack.empty()) {
        // Push current state for redo
        redoStack.push(textBuffer);

        // Restore previous state from undo stack
        textBuffer = undoStack.top();
        undoStack.pop();
    }
}

void redo() {
    if (!redoStack.empty()) {
        // Push current state for undo
        undoStack.push(textBuffer);

        // Restore state from redo stack
        textBuffer = redoStack.top();
        redoStack.pop();
    }
}
