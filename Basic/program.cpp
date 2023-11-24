/*
 * File: program.cpp
 * -----------------
 * This file is a stub implementation of the program.h interface
 * in which none of the methods do anything beyond returning a
 * value of the correct type.  Your job is to fill in the bodies
 * of each of these methods with an implementation that satisfies
 * the performance guarantees specified in the assignment.
 */

#include "program.hpp"



Program::Program() = default;

Program::~Program() = default;

void Program::clear() {
    programLines.clear();
    for (auto it = programStates.begin(); it != programStates.end(); it++) {
        delete it->second;
    }
    programStates.clear();
}

void Program::addSourceLine(int lineNumber, const std::string &line) {
    auto it = programLines.find(lineNumber);
    if (it != programLines.end()) programLines.erase(lineNumber);
    programLines.insert(std::pair<int, std::string>(lineNumber, line));
    if (programStates.count(lineNumber) == 1) {
        delete programStates[lineNumber];
        programStates.erase(lineNumber);
    }
}

void Program::removeSourceLine(int lineNumber) {
    auto it = programLines.find(lineNumber);
    if (it == programLines.end()) return;
    programLines.erase(lineNumber);
    auto iter = programStates.find(lineNumber);
    if (iter == programStates.end()) return;
    programStates.erase(lineNumber);
}

std::string Program::getSourceLine(int lineNumber) {
    auto it = programLines.find(lineNumber);
    if (it == programLines.end()) return "";
    else return it->second;
}

void Program::setParsedStatement(int lineNumber, Statement *&stmt) {
    auto it = programLines.find(lineNumber);
    if (it != programLines.end()) {
        auto iter = programStates.find(lineNumber);
        if (iter != programStates.end()) {
            delete programStates[lineNumber];
            programStates.erase(lineNumber);
        }
        TokenScanner scanner;
        scanner.ignoreWhitespace();
        scanner.scanNumbers();
        scanner.setInput(programLines[lineNumber]);
        std::string token = scanner.nextToken();
        if (token == "REM") {
            int x = scanner.getPosition();
            std::string str = programLines[lineNumber].substr(x);
            stmt = new REM_stmt(str);
        } else if (token == "LET") {
            Expression *exp = parseExp(scanner);
            stmt = new LET_stmt(exp);
        } else if (token == "PRINT") {
            Expression *exp = parseExp(scanner);
            stmt = new PRINT_stmt(exp);
        } else if (token == "INPUT") {
            std::string str = scanner.nextToken();
            stmt = new INPUT_stmt(str);;
        } else if (token == "END") {
            stmt = new END_stmt;
        } else if (token == "GOTO") {
            std::string str = scanner.nextToken();
            int num = stringToInteger(str);
            stmt = new GOTO_stmt(num);
        } else if (token == "IF") {
            int start = scanner.getPosition();
            while (token != "THEN") {
                token = scanner.nextToken();
            }
            int end = scanner.getPosition();
            int num = stringToInteger(programLines[lineNumber].substr(end));
            std::string str = programLines[lineNumber].substr(start, end - start - 5);
            stmt = new IF_stmt(str, num);
        }
        programStates.insert(std::pair<int, Statement*>(lineNumber, stmt));
    }
}


Statement *Program::getParsedStatement(int lineNumber) {
    auto it = programStates.find(lineNumber);
    if (it != programStates.end()) return programStates[lineNumber];
    else return nullptr;
}

int Program::getFirstLineNumber() {
    if (programLines.empty()) return -1;
    auto it = programLines.begin();
    return it->first;
}

int Program::getNextLineNumber(int lineNumber) {
    if (currentLine != 0) {
        int num = currentLine;
        currentLine = 0;
        return num;
    }
    auto it = programLines.upper_bound(lineNumber);
    if (it == programLines.end()) return -1;
    return it->first;
}
