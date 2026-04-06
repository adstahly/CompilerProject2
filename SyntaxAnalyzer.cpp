#include <iostream>
#include <map>
#include <vector>
#include <fstream>
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
    string current_token() {
        if (tokitr != tokens.end()) return *tokitr;
        return "EOF (End of File)";
    }

    bool vdec() {
        cout << "[TRACE] Entering vdec(). Looking at: " << current_token() << endl;
        if (tokitr != tokens.end() && *tokitr == "t_var") {
            tokitr++;
            lexitr++;
            if (tokitr != tokens.end() && vars()) {
                while (tokitr != tokens.end() && vars()) {}
            } else {
                return false;
            }
        }
        cout << "[TRACE] vdec() finished (Epsilon path). Moving on." << endl;
        return true;
    }

    bool stmtlist() {
        cout << "[TRACE] Entering stmtlist(). Looking at: " << current_token() << endl;
        if (tokitr != tokens.end() && *tokitr != "t_end") {
            int status = stmt();
            cout << "[TRACE] Returned from first stmt(). Lookahead is now: " << current_token() << endl;

            while (tokitr != tokens.end() && status == 1) {
                status = stmt();
            }
            if (status == -1) {
                return false;
            }
        }
        return true;
    }



    bool ifstmt() {
    cout << "[TRACE] Entering ifstmt(). Current: " << *tokitr << endl;

    if (tokitr != tokens.end() && *tokitr == "t_if") {
        tokitr++; lexitr++;
        if (tokitr != tokens.end() && *tokitr == "s_lparen") {
            cout << "[TRACE] Found '('. Parsing logical expression..." << endl;
            tokitr++; lexitr++;

            if (tokitr != tokens.end() && logexpr()) {
                cout << "[TRACE] logexpr() returned true. Looking for ')'." << endl;

                if (tokitr != tokens.end() && *tokitr == "s_rparen") {
                    tokitr++; lexitr++;
                    if (tokitr != tokens.end() && *tokitr == "t_then") {
                        cout << "[TRACE] Found 'then'. Entering stmtlist()." << endl;
                        tokitr++; lexitr++;

                        if (tokitr != tokens.end() && stmtlist()) {
                            cout << "[TRACE] Returned from 'then' block stmtlist(). Checking elsepart()." << endl;

                            if (tokitr != tokens.end() && elsepart()) {
                                cout << "[TRACE] elsepart() handled. Looking for 'end' of if block." << endl;

                                if (tokitr != tokens.end() && *tokitr == "t_end") {
                                    tokitr++; lexitr++;
                                    cout << "[TRACE] Found 'end'. Checking for mandatory trailing 'if'." << endl;

                                    if (tokitr != tokens.end() && *tokitr == "t_if") {
                                        cout << "[TRACE] Found trailing 'if'. ifstmt() SUCCESS." << endl;
                                        tokitr++; lexitr++;
                                        return true;
                                    } else {
                                        cout << "[ERROR] ifstmt expected 'if' after 'end', found: " << (tokitr != tokens.end() ? *tokitr : "EOF") << endl;
                                    }
                                } else {
                                    cout << "[ERROR] ifstmt expected 'end', found: " << (tokitr != tokens.end() ? *tokitr : "EOF") << endl;
                                }
                            }
                        }
                    } else {
                        cout << "[ERROR] ifstmt expected 'then', found: " << (tokitr != tokens.end() ? *tokitr : "EOF") << endl;
                    }
                } else {
                    cout << "[ERROR] ifstmt expected ')', found: " << (tokitr != tokens.end() ? *tokitr : "EOF") << endl;
                }
            }
        }
    }
    return false;
}

    bool elsepart() {
        if (tokitr != tokens.end() && *tokitr == "t_else") {
            tokitr++; lexitr++;
            if (tokitr != tokens.end() && stmtlist()) {} else {return false;}
        }
        return true;
    }

    bool assignstmt() {
    cout << "[TRACE] Entering assignstmt(). Current lexeme: " << *lexitr << endl;

    if (tokitr != tokens.end() && symboltable.contains(*lexitr)) {
        string storedType = symboltable[*lexitr];
        cout << "[TRACE] ID found in Symbol Table: " << *lexitr << " | Stored Type: " << storedType << endl;

        tokitr++;
        lexitr++;

        if (tokitr != tokens.end() && *tokitr == "s_assign") {
            cout << "[TRACE] Found assignment operator (=). Moving to expression." << endl;
            tokitr++;
            lexitr++;

            // --- THE TYPE CHECKING LOGIC ---
            if (storedType == "t_integer") {
                cout << "[TRACE] Path: Integer. Calling arithexpr()." << endl;
                if (!arithexpr()) {
                    cout << "[ERROR] arithexpr() failed for integer assignment." << endl;
                    return false;
                }
            }
            else if (storedType == "t_string") {
                cout << "[TRACE] Path: String. Calling strterm()." << endl;
                if (!strterm()) {
                    cout << "[ERROR] strterm() failed for string assignment." << endl;
                    return false;
                }
            }
            else {
                cout << "[ERROR] Unknown type '" << storedType << "' found in symbol table for " << *prev(lexitr, 2) << endl;
                return false;
            }

            // --- THE SEMICOLON CHECK ---
            if (tokitr != tokens.end() && *tokitr == "s_semi") {
                cout << "[TRACE] Semicolon found. Assignment Statement SUCCESS." << endl;
                tokitr++;
                lexitr++;
                return true;
            } else {
                string found = (tokitr != tokens.end()) ? *tokitr : "EOF";
                cout << "[ERROR] Missing semicolon after assignment. Found: " << found << endl;
            }
        } else {
            cout << "[ERROR] Expected '=' after identifier " << *prev(lexitr) << endl;
        }
    } else {
        cout << "[ERROR] Identifier '" << *lexitr << "' not found in Symbol Table (Undeclared variable)." << endl;
    }
    return false;
}

    bool outputstmt() {
        cout << "[TRACE] Entering outputstmt(). Current token: " << *tokitr << endl;

        if (tokitr != tokens.end() && *tokitr == "t_output") {
            tokitr++;
            lexitr++;

            if (tokitr != tokens.end() && *tokitr == "s_lparen") {
                cout << "[TRACE] Found '('. Checking for expression or string." << endl;
                tokitr++;
                lexitr++;

                // Bookmark the start of the content
                auto bookmark= tokitr;

                cout << "[TRACE] Attempting Path A: numterm()..." << endl;
                if (tokitr != tokens.end() && numterm()) {
                    cout << "[TRACE] Path A (numterm) SUCCESS." << endl;
                }
                else {
                    // BACKTRACK
                    cout << "[TRACE] Path A FAILED. Resetting iterators to try Path B: strterm()." << endl;
                    tokitr = bookmark;
                    lexitr = bookmark;

                    if (tokitr != tokens.end() && strterm()) {
                        cout << "[TRACE] Path B (strterm) SUCCESS." << endl;
                    } else {
                        cout << "[ERROR] outputstmt FAILED: Neither numterm nor strterm matched content." << endl;
                        return false;
                    }
                }

                // Check for the closing parenthesis
                if (tokitr != tokens.end() && *tokitr == "s_rparen") {
                    cout << "[TRACE] Found ')'. outputstmt() is VALID." << endl;
                    tokitr++;
                    lexitr++;
                    return true;
                } else {
                    string found = (tokitr != tokens.end()) ? *tokitr : "EOF";
                    cout << "[ERROR] outputstmt expected ')', but found: " << found << endl;
                }
            }
        }
        return false;
    }

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
    }



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
                if (!isdigit(num[i])) {
                    return false;
                }
            }
            tokitr++; lexitr++;
            return true;
        }
        return false;
    }


    bool logicop() {
        if (tokitr != tokens.end() && (*tokitr == "t_and" || *tokitr == "t_or")) {
            tokitr++; lexitr++;
            return true;
        }
        return false;
    }
    bool vars() {
        string varType = type();
        if (tokitr != tokens.end() && !varType.empty()) {
            if (tokitr != tokens.end() && *tokitr == "t_id") {
                symboltable[*lexitr] = varType;
                tokitr++; lexitr++;
                while (tokitr != tokens.end() && *tokitr == "s_comma") {
                    tokitr++; lexitr++;
                    if (tokitr != tokens.end() && *tokitr == "t_id") {
                        if (!symboltable.contains(*lexitr)) {
                            symboltable[*lexitr] = varType;
                            tokitr++; lexitr++;
                        } else {
                            return false;
                        }
                    }
                }
                if (tokitr != tokens.end() && *tokitr == "t_semi") {
                    tokitr++; lexitr++;
                    return true;
                }

            }
        }
        return false;
    }
    string type() {
        if (tokitr != tokens.end() && (*tokitr == "t_integer" || *tokitr == "t_string")) {
            string type = *tokitr;
            tokitr++; lexitr++;
            return type;
        }
        return "";
    }
    int stmt() {
        cout << "[TRACE] Entering stmt() STUB. Consuming: " << current_token() << endl;
        if (*tokitr == "t_if") return ifstmt() ? 1 : -1;
        if (*tokitr == "t_while") return whilestmt() ? 1 : -1;
        if (*tokitr == "t_id") return assignstmt() ? 1 : -1;
        if (*tokitr == "t_input") return inputstmt() ? 1 : -1;
        if (*tokitr == "t_output") return outputstmt() ? 1 : -1;
        return 0;
    }
    bool whilestmt() {
        tokitr++; lexitr++;
        return true;
    }
    bool inputstmt() {
        if (tokitr != tokens.end() && *tokitr == "t_input") {
            tokitr++; lexitr++;
            if (tokitr != tokens.end() && *tokitr == "s_lparen") {
                tokitr++; lexitr++;
                if (tokitr != tokens.end() && *tokitr == "t_id") {
                    if (symboltable.contains(*lexitr)) {
                        tokitr++; lexitr++;
                        if (tokitr != tokens.end() && *tokitr == "s_rparen") {
                            tokitr++; lexitr++;
                            return true;
                        }
                    }
                }
            }
        }
        return false;
    }
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
        cout << "[TRACE] Entering relexpr()" << endl;
        if (arithexpr()) {
            if (relop()) {
                if (arithexpr()) {
                    cout << "[TRACE] relexpr() SUCCESS" << endl;
                    return true;
                }
            }
        }
        return false;
    }
    bool strterm() {
        tokitr++; lexitr++;
        return true;
    }
    bool relop() {
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
    bool arithop() {
        if (tokitr != tokens.end()) {
            if (string lex = *lexitr; lex == "+" || lex == "-" || lex == "*" || lex == "/" || lex == "%") {
                tokitr++; lexitr++;
                return true;
            }
        }
        return false;
    }

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
        tokitr = tokens.begin();
        lexitr = lexemes.begin();

        cout << "[TRACE] Starting parse(). First token: " << current_token() << endl;

        if (vdec()) {
            cout << "[TRACE] Returned to parse() after vdec(). Looking for t_main. Current: " << current_token() << endl;

            if (tokitr != tokens.end() && *tokitr == "t_main") {
                tokitr++;
                lexitr++;
                cout << "[TRACE] Found t_main. Consumed it. Calling stmtlist(). Current: " << current_token() << endl;

                if (stmtlist()) {
                    cout << "[TRACE] Returned to parse() after stmtlist(). Looking for t_end. Current: " << current_token() << endl;

                    // NOTE: Added trace here to show the exact evaluation
                    if (tokitr == tokens.end()) {
                        cout << "[TRACE] WARNING: tokitr is at tokens.end() before checking for t_end!" << endl;
                    }

                    if (tokitr != tokens.end() && *tokitr == "t_end") {
                        cout << "[TRACE] Found t_end! Printing symbol table." << endl;
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

        // SAFELY print the error without dereferencing .end()
        cout << "\n[ERROR] Syntax parsing failed." << endl;
        if (tokitr != tokens.end()) {
            cout << "Error at Line: " << *lexitr << " " << *tokitr << endl;
        } else {
            cout << "Error triggered at the End of File (Iterators are out of bounds)." << endl;
        }
        return false;
    };
};

int main() {

    ifstream infile("input.txt");
    if (!infile) {
        cout << "Error opening file" << endl;
        return 1;
    }

    SyntaxAnalyzer syntax_analyzer(infile);

    if (syntax_analyzer.parse()) {
        cout << "Input is Valid" << endl;
    }
    return 0;
}
