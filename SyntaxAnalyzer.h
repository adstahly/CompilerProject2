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
    bool vdec();
    int vars();
    bool stmtlist();
    int stmt();
    bool ifstmt();
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
        string lexeme, token;
        while (infile >> lexeme >> token) {
            lexemes.push_back(lexeme);
            tokens.push_back(token);
        }
    }
    // pre: 1st parameter consists of an open file containing a source code's
    //	valid scanner/lexical analyzer output.  This data must be in the form: token : lexeme
    // post: the vectors have been populated

    bool parse() { // no true false not
        //no var
        vdec();
        if (tokitr != tokens.end() && *tokitr != "main") {
            stmtlist();
        }

        stmtlist();

    }
    // pre: none
    // post: The lexemes/tokens have been parsed.
    // If an error occurs, a message prints indicating the token/lexeme pair
    // that caused the error.
    // If no error, vectors contain syntactically correct source code
};


#endif //COMPILERPART2_SYNTAXANALYZER_H