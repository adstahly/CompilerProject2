//Adam Stahly
//Derek Schober
#include <fstream>
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
        if (tokitr != tokens.end() && *tokitr == "t_var") {
            tokitr++;
            lexitr++;
            if (tokitr != tokens.end() && vars()) {
                while (tokitr != tokens.end() && vars()) {}
                return true;
            }
            return false;
        }
        return true;
    };

    bool vars() {
        // includes type
        string type = "";

        if (tokitr != tokens.end() && (*tokitr == "t_integer" || *tokitr == "t_string")) {
            type = *tokitr;
            tokitr++;
            lexitr++;
        } else
            return false;
        if (tokitr != tokens.end() && *tokitr == "t_id") {
            if (!symboltable.contains(*lexitr)) {
                symboltable[*lexitr] = type;
                tokitr++;
                lexitr++;
            } else
                return false;
            while (tokitr != tokens.end() && *tokitr == "s_comma") {
                tokitr++;
                lexitr++;
                if (tokitr != tokens.end() && *tokitr == "t_id") {
                    if (!symboltable.contains(*lexitr)) {
                        symboltable[*lexitr] = type;
                        tokitr++; lexitr++;
                    } else
                        return false;
                }
            }
        }
        if (tokitr != tokens.end() && *tokitr == "s_semi") {
            tokitr++;
            lexitr++;
            return true;
        }
        return true;
    };

    bool stmtlist() {
        while (tokitr != tokens.end() && *tokitr != "t_end" && *tokitr != "t_else") {

            if (!stmt())
                return false;
        }
        return true;
    }

    bool stmt() {
        if (tokitr == tokens.end()) return false;
        if (*tokitr == "t_if") return ifstmt();
        if (*tokitr == "t_while") return whilestmt();
        if (*tokitr == "t_id") return assignstmt();
        if (*tokitr == "t_input") return inputstmt();
        if (*tokitr == "t_output") return outputstmt();
        return false;
    };

    bool ifstmt() {
        if (tokitr != tokens.end() && *tokitr == "t_if") {
            tokitr++; lexitr++;
            if (tokitr != tokens.end() && *tokitr == "s_lparen") {
                tokitr++; lexitr++;
                if (tokitr != tokens.end() && logexpr()) {
                    if (tokitr != tokens.end() && *tokitr == "s_rparen") {
                        tokitr++; lexitr++;
                        if (tokitr != tokens.end() && *tokitr == "t_then") {
                            tokitr++; lexitr++;
                            if (tokitr != tokens.end() && stmtlist()) {
                                if (tokitr != tokens.end() && elsepart()) {
                                    if (tokitr != tokens.end() && *tokitr == "t_end") {
                                        tokitr++; lexitr++;
                                        if (tokitr != tokens.end() && *tokitr == "t_if") {
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
        }
        return false;
    };

    bool elsepart() {
        if (tokitr != tokens.end() && *tokitr == "t_else") {
            tokitr++; lexitr++;
            if (tokitr != tokens.end() && stmtlist()) {} else {return false;}
        }
        return true;
    };

    bool whilestmt() {
        if (tokitr != tokens.end() && *tokitr == "t_while") {
            tokitr++; lexitr++;
            if (tokitr != tokens.end() && *tokitr == "s_lparen") {
                tokitr++; lexitr++;
                if (logexpr()) {
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

    bool assignstmt() {
        if (tokitr != tokens.end() && symboltable.contains(*lexitr)) {
            string type = symboltable[*lexitr];
            tokitr++;
            lexitr++;
            if (tokitr != tokens.end() && *tokitr == "s_assign") {
                tokitr++;
                lexitr++;
                if (type == "t_integer") {
                    if (!arithexpr()) return false;
                } else if (type == "t_string") {
                    if (!strterm()) return false;
                }
                if (tokitr != tokens.end() && *tokitr == "s_semi") {
                    tokitr++;
                    lexitr++;
                    return true;
                }
            }
        }
        return false;
    };

    bool inputstmt() {
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

    bool outputstmt() {
        if (tokitr != tokens.end() && *tokitr == "t_output") {
            tokitr++;
            lexitr++;
            if (tokitr != tokens.end() && *tokitr == "s_lparen") {
                tokitr++;
                lexitr++;

                auto bookmarkTok = tokitr;
                auto bookmarkLex = lexitr;

                if (tokitr != tokens.end() && numterm()) {
                } else {
                    tokitr = bookmarkTok;
                    lexitr = bookmarkLex;
                    if (!strterm()) {
                        return false;
                    }
                }
                if (tokitr != tokens.end() && *tokitr == "s_rparen") {
                    tokitr++;
                    lexitr++;
                    return true;
                }
            }
        }
        return false;
    };

    bool numterm() {
        if (tokitr != tokens.end()) {
            if (symboltable.contains(*lexitr)) {
                if (symboltable[*lexitr] == "t_integer") {
                    tokitr++; lexitr++;
                    return true;
                }
            }
            string num = *lexitr;
            for (int i = 0; i < num.length(); i++) {
                if (!isdigit(num[i])) {
                    return false;
                }
            }
            tokitr++; lexitr++;
            return true;
        }
        return false;
    };

    bool arithexpr() {
        if (numterm()) {
            while (arithop()) {
                if (!numterm()) {
                    return false;
                }
            }
            return true;
        }
        return false;
    }

    bool relexpr() {
        if (arithexpr())
            if (relop())
                if (arithexpr())
                    return true;
        return false;
    };

    bool logexpr() {
        if (tokitr != tokens.end() && relexpr()) {
            while (tokitr != tokens.end() && logicop()) {
                if (tokitr != tokens.end() && !relexpr()) {
                    return false;
                }
            }
            return true;
        }
        return false;
    };

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

    bool logicop() {
        if (tokitr != tokens.end() && (*tokitr == "t_and" || *tokitr == "t_or")) {
            tokitr++; lexitr++;
            return true;
        }
        return false;
    };

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

    bool arithop() {
        if (tokitr != tokens.end()) {
            string tok = *tokitr;
            if (tok == "s_plus" || tok == "s_minus" || tok == "s_mult" || tok == "s_div" || tok == "s_mod") {
                tokitr++; lexitr++;
                return true;
            }
        }
        return false;
    };

public:
    SyntaxAnalyzer(istream &infile) {
        lexemes.clear();
        tokens.clear();
        string lexeme, token, line;
        while (getline(infile, line)) {
            int idx = -1;
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
        lexitr = lexemes.begin();
        tokitr = tokens.begin();
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
                    if (tokitr != tokens.end() && *tokitr == "t_end") {
                        tokitr++; lexitr++;
                        if (tokitr == tokens.end()) {
                            for (auto const& [id, type] : symboltable) {
                                cout << id << " " << type << endl;
                            }
                            return true;
                        }
                    }
                }
            }
        }
        cout << "Error at Line: " << *lexitr << " " << *tokitr << endl;
        return false;
    };
};

int main() {
    ifstream infile("./input.txt");


    if (SyntaxAnalyzer syntax_analyzer(infile); syntax_analyzer.parse()) {
        cout << "Input is Valid" << endl;
    }

    return 0;
}