#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "scanner.h"

#define DEBUG 0
#define DEBUG_PRINT(fmt, ...) \
    do { if (DEBUG) fprintf(stderr, fmt, ##__VA_ARGS__); } while (0)

char* lexeme;
int Ival;
int is_comment = 0;
char buf[4000]; 
int i = 0;
int lineNum = 1;

void check_comment(char* ch);
int key_or_id(char*);

int get_token() {
    char ch;
    i = 0;

    while ((ch = getchar()) != EOF) {
        check_comment(&ch);
        if (ch == 10) {
            lineNum += 1;
        }
        if (isspace(ch) || is_comment) {
            continue;
        }

        if (isdigit(ch)) {
            DEBUG_PRINT("[SCANNER] Detected digit: %c\n", ch);
            buf[i++] = ch;
            while (isdigit(ch = getchar())) {
                buf[i++] = ch;
            }
            ungetc(ch, stdin);
            buf[i] = '\0';
            Ival = atoi(buf);
            lexeme = buf;
            DEBUG_PRINT("[SCANNER] Integer constant found: %s\n", buf);
            return INTCON;
        }

        if (isalpha(ch) || ch == '_') {
            buf[i++] = ch;
            while (isalnum(ch = getchar()) || ch == '_') {
                buf[i++] = ch;
            }
            ungetc(ch, stdin);
            buf[i] = '\0';
            DEBUG_PRINT("[SCANNER] Identifier or keyword: %s\n", buf);
            return key_or_id(buf);
        }

        if (ch == '=') {
            if ((ch = getchar()) == '=') {
                lexeme = "==";
                return opEQ;
            }
            ungetc(ch, stdin);
            lexeme = "=";
            return opASSG;
        } else if (ch == '+') {
            lexeme = "+";
            return opADD;
        } else if (ch == '-') {
            lexeme = "-";
            return opSUB;
        } else if (ch == '*' && !is_comment) {
            lexeme = "*";
            return opMUL;
        } else if (ch == '/') {
            lexeme = "/";
            return opDIV;
        } else if (ch == '>') {
            if ((ch = getchar()) == '=') {
                lexeme = ">=";
                return opGE;
            }
            ungetc(ch, stdin);
            lexeme = ">";
            return opGT;
        } else if (ch == '<') {
            if ((ch = getchar()) == '=') {
                lexeme = "<=";
                return opLE;
            }
            ungetc(ch, stdin);
            lexeme = "<";
            return opLT;
        } else if (ch == '!') {
            if ((ch = getchar()) == '=') {
                lexeme = "!=";
                return opNE;
            }
            ungetc(ch, stdin);
            lexeme = "!";
            return UNDEF;
        } else if (ch == '|') {
            if ((ch = getchar()) == '|') {
                lexeme = "||";
                return opOR;
            }
            ungetc(ch, stdin);
        } else if (ch == '&') {
            if ((ch = getchar()) == '&') {
                lexeme = "&&";
                return opAND;
            }
            ungetc(ch, stdin);
        }

        if (ch == '(') {
            lexeme = "(";
            return LPAREN;
        } else if (ch == ')') {
            lexeme = ")";
            return RPAREN;
        } else if (ch == '{') {
            lexeme = "{";
            return LBRACE;
        } else if (ch == '}') {
            lexeme = "}";
            return RBRACE;
        } else if (ch == ',') {
            lexeme = ",";
            return COMMA;
        } else if (ch == ';') {
            lexeme = ";";
            return SEMI;
        }
    }
    if (is_comment) {
        fprintf(stderr, "Error: EOF reached inside a comment at line %d\n", lineNum);
        exit(1);
    }
    return EOF;
}

void check_comment(char* ch) {
    if (!is_comment) {
        if (*ch == '/') {
            char next = getchar();
            if (next == '*') {
                is_comment = 1;
                *ch = ' ';
                DEBUG_PRINT("[COMMENT] Entering comment block\n");
            } else {
                ungetc(next, stdin);
            }
        }
    } else {
        if (*ch == '*') {
            char next = getchar();
            if (next == '/') {
                is_comment = 0;
                *ch = ' ';
                DEBUG_PRINT("[COMMENT] Exiting comment block\n");
            } else {
                ungetc(next, stdin);
            }
        }
    }
}

int key_or_id(char* buf) {
    if (strcmp(buf, "int") == 0) {
        lexeme = "int";
        return kwINT;
    }
    if (strcmp(buf, "if") == 0) {
        lexeme = "if";
        return kwIF;
    }
    if (strcmp(buf, "else") == 0) {
        lexeme = "else";
        return kwELSE;
    }
    if (strcmp(buf, "while") == 0) {
        lexeme = "while";
        return kwWHILE;
    }
    if (strcmp(buf, "return") == 0) {
        lexeme = "return";
        return kwRETURN;
    }
    lexeme = buf;
    DEBUG_PRINT("[SCANNER] Identifier detected: %s\n", buf);
    return ID;
}
