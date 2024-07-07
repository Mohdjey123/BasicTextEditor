#ifndef TEXTEDITOR_H
#define TEXTEDITOR_H

#include <vector>
#include <string>

// Function declarations
void displayText(int startLine, int endLine);
void handleArrowKeys(int key, int& startLine, int& endLine);
void insertCharacter(char ch);
void deleteCharacter();
void saveToFile(const std::string& filename);
void loadFromFile(const std::string& filename);
void search(const std::string& query, int& startLine, int& endLine);
void replace(const std::string& target, const std::string& replacement);
void undo();
void redo();

// Variables
extern std::vector<std::string> textBuffer;
extern int cursorX, cursorY;
extern bool insertMode;

#endif // TEXTEDITOR_H
