/*
 * File: statement.cpp
 * -------------------
 * This file implements the constructor and destructor for
 * the Statement class itself.  Your implementation must do
 * the same for the subclasses you define for each of the
 * BASIC statements.
 */

#include "statement.hpp"
#include <iostream>

/* Implementation of the Statement class */

int stringToInt(std::string str);

Statement::Statement() = default;

Statement::~Statement() = default;

// RemStatement - just consumes the rest of the line
RemStatement::RemStatement(TokenScanner &scanner) {
    while (scanner.hasMoreTokens()) {
        scanner.nextToken();
    }
}

void RemStatement::execute(EvalState &state, Program &program) {
    // REM does nothing
}

// LetStatement - parses assignment expression
LetStatement::LetStatement(TokenScanner &scanner) {
    exp = parseExp(scanner);
}

LetStatement::~LetStatement() {
    delete exp;
}

void LetStatement::execute(EvalState &state, Program &program) {
    exp->eval(state);
}

// PrintStatement - parses and prints expression
PrintStatement::PrintStatement(TokenScanner &scanner) {
    exp = parseExp(scanner);
}

PrintStatement::~PrintStatement() {
    delete exp;
}

void PrintStatement::execute(EvalState &state, Program &program) {
    std::cout << exp->eval(state) << std::endl;
}

// InputStatement - reads variable name and prompts for input
InputStatement::InputStatement(TokenScanner &scanner) {
    var = scanner.nextToken();
    if (scanner.getTokenType(var) != WORD) {
        error("SYNTAX ERROR");
    }
    if (scanner.hasMoreTokens()) {
        error("SYNTAX ERROR");
    }
}

void InputStatement::execute(EvalState &state, Program &program) {
    std::cout << " ? ";
    std::string line;
    int value;
    while (true) {
        if (!getline(std::cin, line)) {
            exit(0);
        }
        TokenScanner inputScanner;
        inputScanner.ignoreWhitespace();
        inputScanner.scanNumbers();
        inputScanner.setInput(line);
        std::string token = inputScanner.nextToken();
        
        // Handle negative numbers
        bool isNegative = false;
        if (token == "-") {
            isNegative = true;
            token = inputScanner.nextToken();
        }
        
        if (inputScanner.getTokenType(token) == NUMBER && !inputScanner.hasMoreTokens()) {
            value = stringToInteger(token);
            if (isNegative) value = -value;
            break;
        }
        std::cout << "INVALID NUMBER" << std::endl;
        std::cout << " ? ";
    }
    state.setValue(var, value);
}

// EndStatement - terminates program
EndStatement::EndStatement(TokenScanner &scanner) {
    if (scanner.hasMoreTokens()) {
        error("SYNTAX ERROR");
    }
}

void EndStatement::execute(EvalState &state, Program &program) {
    state.setValue("__END__", 1);
}

// GotoStatement - jumps to line
GotoStatement::GotoStatement(TokenScanner &scanner) {
    std::string token = scanner.nextToken();
    if (scanner.getTokenType(token) != NUMBER) {
        error("SYNTAX ERROR");
    }
    targetLine = stringToInteger(token);
    if (scanner.hasMoreTokens()) {
        error("SYNTAX ERROR");
    }
}

void GotoStatement::execute(EvalState &state, Program &program) {
    state.setValue("__GOTO__", targetLine);
}

// IfStatement - conditional jump
IfStatement::IfStatement(TokenScanner &scanner) {
    lhs = readE(scanner, 1);
    std::string opToken = scanner.nextToken();
    
    // Handle comparison operators
    if (opToken == "=") {
        op = "=";
    } else if (opToken == "<") {
        std::string next = scanner.nextToken();
        if (next == "=") {
            op = "<=";
        } else if (next == ">") {
            op = "<>";
        } else {
            scanner.saveToken(next);
            op = "<";
        }
    } else if (opToken == ">") {
        std::string next = scanner.nextToken();
        if (next == "=") {
            op = ">=";
        } else {
            scanner.saveToken(next);
            op = ">";
        }
    } else {
        error("SYNTAX ERROR");
    }
    
    rhs = readE(scanner, 1);
    
    std::string thenToken = scanner.nextToken();
    if (thenToken != "THEN") {
        error("SYNTAX ERROR");
    }
    
    std::string lineToken = scanner.nextToken();
    if (scanner.getTokenType(lineToken) != NUMBER) {
        error("SYNTAX ERROR");
    }
    targetLine = stringToInteger(lineToken);
    
    if (scanner.hasMoreTokens()) {
        error("SYNTAX ERROR");
    }
}

IfStatement::~IfStatement() {
    delete lhs;
    delete rhs;
}

void IfStatement::execute(EvalState &state, Program &program) {
    int leftVal = lhs->eval(state);
    int rightVal = rhs->eval(state);
    bool condition = false;
    
    if (op == "=") {
        condition = (leftVal == rightVal);
    } else if (op == "<") {
        condition = (leftVal < rightVal);
    } else if (op == ">") {
        condition = (leftVal > rightVal);
    } else if (op == "<=") {
        condition = (leftVal <= rightVal);
    } else if (op == ">=") {
        condition = (leftVal >= rightVal);
    } else if (op == "<>") {
        condition = (leftVal != rightVal);
    }
    
    if (condition) {
        state.setValue("__GOTO__", targetLine);
    }
}
