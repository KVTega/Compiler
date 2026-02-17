#ifndef CODE_GEN_H
#define CODE_GEN_H

#include "ast.h"

// Entry point
void gen_code(ASTnode* funcNode, scope* funcScope); 

// Statements
void codeGen_stmt(ASTnode* stmtNode);
void codeGen_stmtList(ASTnode* stmtList);
void codeGen_assg(ASTnode* assignNode);
void codeGen_if(ASTnode* ifNode);
void codeGen_while(ASTnode* whileNode);
void codeGen_ret(ASTnode* returnNode);

// Expressions
void codeGen_expr(ASTnode* exprNode);
void codeGen_exprList(ASTnode* exprList);
void codeGen_param(ASTnode* paramNode);
void codeGen_bool(ASTnode* boolExpr, char* trueLabel, char* falseLabel);
Quad* codeGen_relop(NodeType relopType, char* targetLabel);

// Helpers
Quad* newinstr(opType op, sym* src1, sym* src2, int src1num, int src2num, sym* dest, char* label);
Quad* newlabel();
sym* newtemp(); 
int get_offsets(ASTnode* funcNode);

// MIPS generation
void gen_mips(Quad* codeHead, int numLocals);
void printSrc(sym* variable);
void printIf(Quad* ifInstr);
void print_ln();
void gen_globals(scope* globalScope);

#endif

