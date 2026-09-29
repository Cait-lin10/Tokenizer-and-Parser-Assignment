// Parser

#include <iostream>
#include <fstream>
#include <cstring>
#include <string>
#include "toyTokenizer.cpp"
using namespace std;

// =====================================================================
// PARSER — recursive descent following the BNF
// =====================================================================

struct Parser {
    Token* tokens;
    int count;
    int pos;
};

const Token* prCurrent(const Parser &p) { return &p.tokens[p.pos]; }

bool prCheck(const Parser &p, TokenType t) { return prCurrent(p)->tokenType == t; }

void prAdvance(Parser &p) {
p.pos++;
}

bool prMatch(Parser &p, TokenType t) {
if (prCheck(p, t)) { prAdvance(p); return true; }
return false;
}

void prError(const Parser &p, string msg) {
const Token* t = prCurrent(p);

cerr << "Parser error at line " << t->line << ": column " << t->column << ": "
    << msg << " (got " << tokenTypeName(t->tokenType) << ")\n";

exit(1);
}

const Token* prExpect(Parser &p, TokenType t, string what) {
if (!prCheck(p, t)) {
prError(p, "expected " + what + " (" + tokenTypeName(t) + ")");
}

const Token* tok = prCurrent(p);
prAdvance(p);
return tok;
}

// =====================================================
// Grammar Functions
//
//
//
//
//
//
//
// ======================================================

// ------------------ <A> -> a<A> | a ---------------

void parseA(Parser &p) {

    int currentLine = prCurrent(p)->line;

    prExpect(p, TOK_a, "a");

    while (prCheck(p, TOK_a) &&
            prCurrent(p)->line == currentLine) {
        prAdvance(p);
    }
}



// ------------------ <B> -> b<B> | b --------------- 

void parseB(Parser &p) {

    int currentLine = prCurrent(p)->line;

    prExpect(p, TOK_b, "b");

    while (prCheck(p, TOK_b) &&
            prCurrent(p)->line == currentLine) {
        prAdvance(p);
    }
}

// ------------------ <C> -> c --------------- 

void parseC(Parser &p) {

    // C must contain exactly one c
    prExpect(p, TOK_c, "c");
}

// ------------------ <S> -> <A><C><B> | <A> --------------- 

void parseS(Parser &p) {

    int currentLine = prCurrent(p)->line;

    parseA(p);

    if (prCheck(p, TOK_c) &&
        prCurrent(p)->line ==currentLine) {
        parseC(p);
        parseB(p);
    }
}

// =====================================================================
// MAIN
// =====================================================================

int main(int argc, char** argv) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <source-file>\n", argv[0]);
        return 1;
    }

    FILE* fp = fopen(argv[1], "r");

    if (!fp) {
        fprintf(stderr, "Cannot open file: %s\n", argv[1]);
        return 1;
    }

    // ---- Tokenize ----

    Tokenizer tk;
    tkInit(&tk, fp);

    Token tokens[MAX_TOKENS];
    int tokenCount = tokenize(&tk, tokens, MAX_TOKENS);

    fclose(fp);

    // -------- Parse --------

    Parser parser;
    parser.tokens = tokens;
    parser.count = tokenCount;
    parser.pos = 0;

    while (!prCheck(parser, TOK_ENDFILE)) {

        int currentLine = prCurrent(parser)->line;

        parseS(parser);

        if (!prCheck(parser, TOK_ENDFILE) &&
            prCurrent(parser)->line == currentLine) {
            prError(parser, "Unexpected token");
        }

        cout << "No errors found on line " << currentLine << "\n";
    }

    return 0;
}
