/*
 * File: Basic.cpp
 * ---------------
 * This file is the starter project for the BASIC interpreter.
 */

#include <cctype>
#include <iostream>
#include <string>
#include "exp.hpp"
#include "parser.hpp"
#include "program.hpp"
#include "Utils/error.hpp"
#include "Utils/tokenScanner.hpp"
#include "Utils/strlib.hpp"



/* Function prototypes */

void processLine(std::string line, Program &program, EvalState &state);

/* Main program */

int main() {
    EvalState state;
    Program program;
    //cout << "Stub implementation of BASIC" << endl;
    while (true) {
        try {
            std::string input;
            getline(std::cin, input);
            if (input.empty())
                return 0;
            processLine(input, program, state);
        } catch (ErrorException &ex) {
            std::cout << ex.getMessage() << std::endl;
        }
    }
    return 0;
}

/*
 * Function: processLine
 * Usage: processLine(line, program, state);
 * -----------------------------------------
 * Processes a single line entered by the user.  In this version of
 * implementation, the program reads a line, parses it as an expression,
 * and then prints the result.  In your implementation, you will
 * need to replace this method with one that can respond correctly
 * when the user enters a program line (which begins with a number)
 * or one of the BASIC commands, such as LIST or RUN.
 */

void processLine(std::string line, Program &program, EvalState &state) {
    TokenScanner scanner;
    scanner.ignoreWhitespace();
    scanner.scanNumbers();
    scanner.setInput(line);

    std::string token = scanner.nextToken();
    if (scanner.getTokenType(token) == NUMBER) {
        int lineNumber = stringToInteger(token);
        int x = scanner.getPosition();
        if (scanner.hasMoreTokens()) {
            program.addSourceLine(lineNumber, line.substr(x));
        } else {
            program.removeSourceLine(lineNumber);
        }
    } else if (scanner.getTokenType(token) == WORD) {
        if (token == "LET") {
          Expression *parsed_stmt = parseExp(scanner);
          LET_stmt let_stmt(parsed_stmt);
          let_stmt.execute(state, program);
        } else if (token == "PRINT") {
            Expression *parsed_stmt = parseExp(scanner);
            PRINT_stmt print_stmt(parsed_stmt);
            print_stmt.execute(state, program);
        } else if (token == "INPUT") {
            std::string str = scanner.nextToken();
            INPUT_stmt input_stmt(str);
            input_stmt.execute(state, program);
        } else if (token == "CLEAR") {
            program.clear();
            state.Clear();
        } else if (token == "QUIT") {
            program.clear();
            state.Clear();
            exit(0);
        } else if (token == "LIST") {
            int num = program.getFirstLineNumber();
            while (num != -1) {
                std::cout<<num<<program.getSourceLine(num)<<'\n';
                num = program.getNextLineNumber(num);
            }
        } else if (token == "RUN") {
            //program.run_stmt(state);
            int lineNumber  = program.getFirstLineNumber();
            while(lineNumber != -1) {
                Statement *stmt;
                program.setParsedStatement(lineNumber, stmt);
                stmt->execute(state, program);
                lineNumber = program.getNextLineNumber(lineNumber);
            }
        }
    }

}

