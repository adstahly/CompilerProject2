//
// Created by Stahl on 3/30/2026.
//

#ifndef COMPILERPART2_SYNTAXANALYZER_H
#define COMPILERPART2_SYNTAXANALYZER_H
#include <iostream>
#include <vector>
#include <string>
#include <map>
using namespace std;

class SyntaxAnalyzer{
private:
    vector<string> lexemes;
    vector<string> tokens;
    vector<string>::iterator lexitr;
    vector<string>::iterator tokitr;
    // map of variables and their datatype
    // i.e. sum t_integer
    map<string, string> symboltable;

    // other private methods
    bool vdec() {
        if (tokitr != tokens.end() && *tokitr == "var") {
            tokitr++; lexitr++;
            if (vars() < 0) {
                return false;
            }
            tokitr++; lexitr++;
            while (tokitr != tokens.end() && vars() == -1) {
                tokitr++; lexitr++;
            }
        } else {
            tokitr++; lexitr++;
            return true;
        }
        return true;
    }
    int vars() {

    }
    bool stmtlist() {
        if (tokitr != tokens.end() && stmt()) {
            tokitr++; lexitr++;
            while (tokitr != tokens.end() && stmt()) {
                tokitr++; lexitr++;
            }
        } else {
           return true;
        }
    }
    int stmt();
    bool ifstmt() {
        if (tokitr != tokens.end() && *tokitr != "if") {
            return false;
        }
        tokitr++; lexitr++;
        if (tokitr != tokens.end() && *tokitr != "(") {
            return false;
        }
        tokitr++; lexitr++;
        if (tokitr != tokens.end() && !expr()) {
            return false;
        }
        if (tokitr != tokens.end() && *tokitr != ")") {
            return false;
        }
        tokitr++; lexitr++;
        if (tokitr != tokens.end() && *tokitr != "then") {
            return false;
        }
        tokitr++; lexitr++;
        if (tokitr != tokens.end() && !stmtlist()) {
            return false;
        }

        if (tokitr != tokens.end() && ) {}
    }
    bool elsepart();
    bool whilestmt();
    bool assignstmt();
    bool inputstmt();
    bool outputstmt();
    bool expr();
    bool simpleexpr();
    bool term();
    bool logicop();
    bool arithop();
    bool relop();

public:
    SyntaxAnalyzer(istream& infile) {
        lexemes.clear();
        tokens.clear();
        string line;
        while (getline(infile, line)) {
            int first = line.find(' ');

            string token = line.substr(0, first);
            string lexeme = line.substr(first + 1);

            lexemes.push_back(token);
            tokens.push_back(lexeme);
        }
    }
    // pre: 1st parameter consists of an open file containing a source code's
    //	valid scanner/lexical analyzer output.  This data must be in the form: token : lexeme
    // post: the vectors have been populated

    bool parse() { // no true false not
        //no var
        tokitr = tokens.begin();
        lexitr = lexemes.begin();
        if (!vdec()) {
            cout << "Error found at this lime: " << *tokitr << " " << *lexitr << endl;
            return false;
        }
        if (tokitr != tokens.end() && *tokitr != "main") {
            cout << "Error found at this lime: " << *tokitr << " " << *lexitr << endl;
            return false;
        }
        if (!stmtlist()) {
            cout << "Error found at this lime: " << *tokitr << " " << *lexitr << endl;
            return false;
        }
        if (tokitr == tokens.end() && *tokitr == "end") {
            return true;
        }
        cout << "Error found at this lime: " << *tokitr << " " << *lexitr << endl;
        return false;
    }
    // pre: none
    // post: The lexemes/tokens have been parsed.
    // If an error occurs, a message prints indicating the token/lexeme pair
    // that caused the error.
    // If no error, vectors contain syntactically correct source code
};


#endif //COMPILERPART2_SYNTAXANALYZER_H