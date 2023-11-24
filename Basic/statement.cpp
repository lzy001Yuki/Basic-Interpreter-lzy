/*
 * File: statement.cpp
 * -------------------
 * This file implements the constructor and destructor for
 * the Statement class itself.  Your implementation must do
 * the same for the subclasses you define for each of the
 * BASIC statements.
 */

#include "statement.hpp"


/* Implementation of the Statement class */

int stringToInt(std::string str);

Statement::Statement() = default;

Statement::~Statement() = default;

//class REMstmt

REM_stmt::REM_stmt(std::string str):comments(str){}

void REM_stmt::execute(EvalState &state, Program &program) {}

//class LETstmt
LET_stmt::LET_stmt(Expression *exp):let(exp){}

LET_stmt::~LET_stmt() {
    delete let;
}

void LET_stmt::execute(EvalState &state, Program &program) {
    let->eval(state);
}

//class PRINTstmt
PRINT_stmt::PRINT_stmt(Expression *exp): print(exp){}

PRINT_stmt::~PRINT_stmt() {
    delete print;
}

void PRINT_stmt::execute(EvalState &state, Program &program) {
    int ans = print->eval(state);
    std::cout<<ans<<'\n';
}

//class INPUTstmt
INPUT_stmt::INPUT_stmt(std::string str): var(str){}

void INPUT_stmt::execute(EvalState &state, Program &program) {
    std::cout<<" ? ";
    std::string value;
    getline(std::cin, value);
    bool flag = false;
    //bool double_flag = false;
    while (!flag) {
        flag = true;
        for (int i = 0; i < value.size(); i++) {
            if (i == 0 && value[0] == '-') {
                continue;
            }
            if (value[i] > '9' || value[i] < '0') {
                flag = false;
                break;
            }
        }
        if (!flag) {
            std::cout<<"INVALID NUMBER"<<'\n';
            std::cout<<" ? ";
            value.clear();
            getline(std::cin, value);
        }
    }
    int val = stringToInteger(value);
    state.setValue(var, val);
}

// class ENDstmt
END_stmt::END_stmt() = default;

void END_stmt::execute(EvalState &state, Program &program) {
    program.currentLine = -1;
}

// class IFstmt
IF_stmt::IF_stmt(std::string str, int num):command(str), Number(num){}

void IF_stmt::execute(EvalState &state, Program &program) {
    TokenScanner scanner;
    scanner.ignoreWhitespace();
    scanner.scanNumbers();
    int left = 0;
    int com_flag;
    for (; left < command.size(); left++) {
        if (command[left] == '<') {
            com_flag = 1;
            break;
        } else if (command[left] == '=') {
            com_flag = 2;
            break;
        } else if (command[left] == '>') {
            com_flag = 3;
            break;
        }
    }
    scanner.setInput(command.substr(0, left));
    Expression *lhs = parseExp(scanner);
    scanner.setInput(command.substr(left + 1));
    Expression *rhs = parseExp(scanner);
    if (com_flag == 1) {
        if (lhs->eval(state) < rhs->eval(state)) program.currentLine = Number;
    } else if (com_flag == 2) {
        if (lhs->eval(state) == rhs->eval(state)) program.currentLine = Number;
    } else if (com_flag == 3) {
        if (lhs->eval(state) > rhs->eval(state)) program.currentLine = Number;
    } else {
        program.currentLine = -1;
    }
    delete lhs;
    delete rhs;
}

// class GOTOstmt
GOTO_stmt::GOTO_stmt(int num): lineNumber(num){};

void GOTO_stmt::execute(EvalState &state, Program &program) {
    auto it = program.programLines.find(lineNumber);
    if (it == program.programLines.end()) {
        std::cout<<"LINE NUMBER ERROR"<<'\n';
        return;
    }
    program.currentLine = lineNumber;
}

