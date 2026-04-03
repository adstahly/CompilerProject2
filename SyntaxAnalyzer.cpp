#include <iostream>
#include <map>
#include <vector>
using namespace std;

class SyntaxAnalyzer {
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
            tokitr++;
            lexitr++;
            if (tokitr != tokens.end() && vars()) {
                while (tokitr != tokens.end() && vars()) {}
            } else {
                return false;
            }
        }
        return true;
    }

    bool vars() {

    }

    bool stmtlist() {
        if (tokitr != tokens.end() && stmt()) {
            while (tokitr != tokens.end() && stmt()) {}
        }
        return true;
    }

    bool stmt() {

    };

    bool ifstmt() {

    }

    bool elsepart();

    bool whilestmt() {

    }

    bool assignstmt();

    bool inputstmt(){ // write
        if (tokitr != tokens.end() && *lexitr == "input") {
            tokitr++; lexitr++;
            if (tokitr != tokens.end() && *lexitr == "(") {
                tokitr++; lexitr++;
                if (tokitr != tokens.end() && symboltable.contains(*lexitr)) {
                    tokitr++; lexitr++;
                    if (tokitr != tokens.end() && *lexitr == ")") {
                        tokitr++; lexitr++;
                        return true;
                    }
                }
            }
        }
        return false;
    };
    bool outputstmt();

    bool expr();

    bool simpleexpr();

    bool numterm() { // write
        if (tokitr != tokens.end()) {
            if (symboltable.contains(*lexitr)) {
                if (symboltable[*lexitr] == "t_integer") {
                    tokitr++; lexitr++;
                    return true;
                }
            }
            string num = *lexitr;
            for (int i = 0; i < num.length(); i++) {
                if (!isalpha(num[i])) {
                    return false;
                }
            }
            return true;
        }
        return false;
    };
    bool strterm();

    bool logicop();

    bool arithop(){ // write
        if (tokitr != tokens.end()) {
            string lex = *lexitr;
            if (lex == ">" || lex == "<") {
                tokitr++; lexitr++;
                if (tokitr != tokens.end() && *lexitr == "=") {
                    tokitr++; lexitr++;
                }
                return true;
            }
            if (lex == "!" || lex == "=") {
                tokitr++; lexitr++;
                if (tokitr != tokens.end() && *lexitr == "=") {
                    tokitr++; lexitr++;
                    return true;
                }
            }
        }
        return false;
    }
    bool relop() {// write
        if (tokitr != tokens.end()) {
            string lex = *lexitr;
            if (lex == "+" || lex == "-" || lex == "*" || lex == "/" || lex == "%") {
                return true;
            }
            return false;
        }
    };

public:
    SyntaxAnalyzer(istream &infile) {
        lexemes.clear();
        tokens.clear();
        string lexeme, token, line;
        int idx = -1;
        while (getline(infile, line)) {
            for (int i = 0; i < line.length(); i++) {
                if (line[i] == ' ') {
                    idx = i;
                    break;
                }
            }
            token = line.substr(0, idx);
            lexeme = line.substr(idx + 1, line.length() - idx - 1);
            lexemes.push_back(lexeme);
            tokens.push_back(token);
        }
    };
    // pre: 1st parameter consists of an open file containing a source code's
    //	valid scanner/lexical analyzer output.  This data must be in the form: token : lexeme
    // post: the vectors have been populated


    bool parse() {
        // pre: none
        // post: The lexemes/tokens have been parsed.
        // If an error occurs, a message prints indicating the token/lexeme pair
        // that caused the error.
        // If no error, vectors contain syntactically correct source code

        // sentence structure -> VDEC main STMTLIST end
        if (vdec()) {
            // check for main
            if (tokitr != tokens.end() && *tokitr == "main") {
                tokitr++;
                lexitr++;
                if (stmtlist()) {
                    // check for end
                    if (tokitr == tokens.end() && *tokitr == "end") {
                        return true;
                    }
                    return true;
                }
            }
        }
        cout << "error: " << *lexitr << " " << *tokitr << endl;
        return false;
    };
};
