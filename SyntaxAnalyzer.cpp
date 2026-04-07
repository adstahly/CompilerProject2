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
    bool vdec();

    bool vars() {
        // includes type
        string type;
        bool error = false;
        if (tokitr != tokens.end() && (*tokitr == "t_integer" || *tokitr == "t_string")) {
            if (*tokitr == "t_integer") {
                type = "t_integer";
            } else {
                type = "t_string";
            }
            tokitr++; lexitr++;
            if (tokitr != tokens.end() && *tokitr == "t_id") {
                if (symboltable.contains(*lexitr)) {
                    if (symboltable[*lexitr] == type) {
                        tokitr++; lexitr++;
                    } else error = true;
                } else {
                    symboltable[*lexitr] = type;
                }
            } else error = true;
        }
        while (tokitr != tokens.end() || *tokitr == "s_comma") {
            tokitr++; lexitr++;
            if (tokitr != tokens.end() && *tokitr == "t_id") {
                if (symboltable.contains(*lexitr)) {
                    if (symboltable[*lexitr] == type) {
                        tokitr++; lexitr++;
                    } else error = true;
                } else {
                    symboltable[*lexitr] = type;
                }
            } else error = true;
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
        if (tokitr != tokens.end() && *tokitr == "t_while") {
            tokitr++; lexitr++;
            if (tokitr != tokens.end() && *tokitr == "s_lparen") {
                tokitr++; lexitr++;
                if (expr()) {
                    if (tokitr != tokens.end() && *tokitr == "s_rparen") {
                        tokitr++; lexitr++;
                        if (tokitr != tokens.end() && *tokitr == "t_loop") {
                            tokitr++; lexitr++;
                            if (stmtlist()) {
                                if (tokitr != tokens.end() && *tokitr == "t_end") {
                                    tokitr++; lexitr++;
                                    if (tokitr != tokens.end() && *tokitr == "t_loop") {
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

    bool inputstmt() {
        // write
        if (tokitr != tokens.end() && *tokitr == "t_input") {
            tokitr++;
            lexitr++;
            if (tokitr != tokens.end() && *tokitr == "s_lparen") {
                tokitr++;
                lexitr++;
                if (tokitr != tokens.end() && symboltable.contains(*lexitr)) {
                    tokitr++; lexitr++;
                    if (tokitr != tokens.end() && *tokitr == "s_rparen") {
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

    bool numterm();

    bool strterm() {
        if (tokitr != tokens.end()) {
            if (symboltable.contains(*lexitr)) {
                if (symboltable[*lexitr] == "t_string") {
                    tokitr++; lexitr++;
                    return true;
                }
            }
            if (*tokitr == "t_text") {
                tokitr++; lexitr++;
                return true;
            }
        }
        return false;
    };

    bool logicop();

    bool relop() {
        if (tokitr != tokens.end()) {
            string tok = *tokitr;
            if (tok == "s_lt" || tok == "s_le" || tok == "s_gt" || tok == "s_ge" || tok == "s_eq" || tok == "s_ne") {
                tokitr++; lexitr++;
                return true;
            }
        }
        return false;
    }

    bool arithop();

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
            if (tokitr != tokens.end() && *tokitr == "t_main") {
                tokitr++; lexitr++;
                if (stmtlist()) {
                    // check for end
                    if (tokitr == tokens.end() && *tokitr == "t_end") {
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