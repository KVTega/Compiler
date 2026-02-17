/*
 * File: scanner.h
 * Author: Saumya Debray
 * Purpose: Lists tokens and their values for use in the CSC 453 project
 */

 #ifndef __SCANNER_H__
 #define __SCANNER_H__
 
 /*
  * The enum Token defines integer values for the various tokens.  These
  * are the values returned by the scanner.
  */
 typedef enum {
   UNDEF     /* 0 undefined */,
   ID        /* 1 identifier: e.g., x, abc, p_q_12 */,
   INTCON    /* 2 integer constant: e.g., 12345 */,
   LPAREN    /* 3 '(' : Left parenthesis */,
   RPAREN    /* 4 ')' : Right parenthesis */,
   LBRACE    /* 5 '{' : Left curly brace */,
   RBRACE    /* 6 '}' : Right curly brace */,
   COMMA     /* 7 ',' : Comma */,
   SEMI      /* 8 ;  : Semicolon */,
   kwINT     /* 9 int */,
   kwIF      /* 10	if */,
   kwELSE    /* 11	else */,
   kwWHILE   /* 12	while */,
   kwRETURN  /* 13	return */,
   opASSG    /* 14	= : Assignment */,
   opADD     /* 15	+ : addition */,
   opSUB     /* 16	– : subtraction */,
   opMUL     /* 17	* : multiplication */,
   opDIV     /* 18	/ : division */,
   opEQ      /* 19	== : Op: equals */,
   opNE      /* 20	!= : op: not-equals */,
   opGT      /* 21	>  : Op: greater-than */,
   opGE      /* 22	>= : Op: greater-or-equal */,
   opLT      /* 23	<  : Op: less-than */,
   opLE      /* 24	<= : Op: less-or-equal */,
   opAND     /* 25	&& : Op: logical-and */,
   opOR      /* 26	|| : Op: logical-or */,
   opNOT     /* 27 ! : Op: logical-not */
 } Token;
 
 // Global variables (extern to avoid multiple definitions)
 extern char* lexeme;        // Pointer to the current lexeme
 extern int Ival;            // Used to store an integer value
 extern int is_comment;      // Flag to track if we are in a comment
 extern char buf[4000];      // Buffer to hold input data
 extern int i;               // Buffer index
 extern int lineNum;         // Line number for tracking
 
 // Function prototypes
 extern void check_comment(char* ch);
 extern int key_or_id(char* buf);
 extern int get_token();     // Function to get next token
 
 #endif  /* __SCANNER_H__ */
 