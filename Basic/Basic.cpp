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
                continue;
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
    
    if (!scanner.hasMoreTokens()) {
        return;
    }
    
    std::string token = scanner.nextToken();
    TokenType type = scanner.getTokenType(token);
    
    // Check if it's a line number
    if (type == NUMBER) {
        int lineNumber = stringToInteger(token);
        if (!scanner.hasMoreTokens()) {
            // Delete line
            program.removeSourceLine(lineNumber);
        } else {
            // Add or replace line
            program.addSourceLine(lineNumber, line);
            // Parse and store the statement
            std::string cmd = scanner.nextToken();
            Statement *stmt = nullptr;
            
            if (cmd == "REM") {
                stmt = new RemStatement(scanner);
            } else if (cmd == "LET") {
                stmt = new LetStatement(scanner);
            } else if (cmd == "PRINT") {
                stmt = new PrintStatement(scanner);
            } else if (cmd == "INPUT") {
                stmt = new InputStatement(scanner);
            } else if (cmd == "END") {
                stmt = new EndStatement(scanner);
            } else if (cmd == "GOTO") {
                stmt = new GotoStatement(scanner);
            } else if (cmd == "IF") {
                stmt = new IfStatement(scanner);
            } else {
                error("SYNTAX ERROR");
            }
            
            if (stmt != nullptr) {
                program.setParsedStatement(lineNumber, stmt);
            }
        }
    } else {
        // Immediate command
        if (token == "RUN") {
            int currentLine = program.getFirstLineNumber();
            while (currentLine != -1) {
                Statement *stmt = program.getParsedStatement(currentLine);
                if (stmt != nullptr) {
                    state.setValue("__GOTO__", -1);
                    stmt->execute(state, program);
                    if (state.isDefined("__GOTO__") && state.getValue("__GOTO__") != -1) {
                        currentLine = state.getValue("__GOTO__");
                        state.setValue("__GOTO__", -1);
                    } else {
                        currentLine = program.getNextLineNumber(currentLine);
                    }
                } else {
                    currentLine = program.getNextLineNumber(currentLine);
                }
            }
        } else if (token == "LIST") {
            int currentLine = program.getFirstLineNumber();
            while (currentLine != -1) {
                std::cout << program.getSourceLine(currentLine) << std::endl;
                currentLine = program.getNextLineNumber(currentLine);
            }
        } else if (token == "CLEAR") {
            program.clear();
            state.Clear();
        } else if (token == "QUIT") {
            exit(0);
        } else if (token == "HELP") {
            // Optional, do nothing
        } else if (token == "REM") {
            RemStatement stmt(scanner);
            stmt.execute(state, program);
        } else if (token == "LET") {
            LetStatement stmt(scanner);
            stmt.execute(state, program);
        } else if (token == "PRINT") {
            PrintStatement stmt(scanner);
            stmt.execute(state, program);
        } else if (token == "INPUT") {
            InputStatement stmt(scanner);
            stmt.execute(state, program);
        } else if (token == "END") {
            EndStatement stmt(scanner);
            stmt.execute(state, program);
        } else if (token == "GOTO") {
            error("SYNTAX ERROR");
        } else if (token == "IF") {
            error("SYNTAX ERROR");
        } else {
            error("SYNTAX ERROR");
        }
    }
}

