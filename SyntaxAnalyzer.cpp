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
            int var;
            var = vars();
            return true;
        }
        return false;
    }

    bool vars() {
        // includes type
        string type;
        bool error = false;
        // TODO: continue statement with ','
        while (tokitr != tokens.end() && (*tokitr == "t_integer" || *tokitr == "t_string") && !error) {
            if (*lexitr == "t_integer") {
                type = "integer";
            } else {
                type = "string";
            }
            tokitr++;
            lexitr++;
            while (tokitr != tokens.end() && *tokitr == "t_id") {
                if (tokitr != tokens.end()) {
                    if (symboltable.contains(*lexitr)) {
                        if (symboltable[*lexitr] == type) {
                            tokitr++;
                            lexitr++;
                        } else error = true;
                    } else {
                        symboltable[*lexitr] = type;
                    }
                } else error = true;
            }
        }
        if (error) return false;
        return true;
    };

    bool stmtlist();

    bool stmt() {
        // write
        if (ifstmt()) return true;
        if (whilestmt()) return true;
        if (assignstmt()) return true;
        if (inputstmt()) return true;
        if (outputstmt()) return true;
        return false;
    };

    bool ifstmt();

    bool elsepart();

    bool whilestmt() {
        // write
        if (tokitr != tokens.end() && *lexitr == "while") {
            tokitr++; lexitr++;
            if (tokitr != tokens.end() && *lexitr == "(") {
                tokitr++; lexitr++;
                if (expr()) {
                    if (tokitr != tokens.end() && *lexitr == ")") {
                        tokitr++; lexitr++;
                        if (tokitr != tokens.end() && *lexitr == "loop") {
                            tokitr++; lexitr++;
                            if (stmtlist()) {
                                if (tokitr != tokens.end() && *lexitr == "end") {
                                    tokitr++; lexitr++;
                                    if (tokitr != tokens.end() && *lexitr == "loop") {
                                        tokitr++; lexitr++;
                                        return true;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        return false;
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
