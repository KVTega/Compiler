#include "scanner.h"
#include <stdio.h>
#include <stdlib.h>
#include "symbol.h"
#include "ast.h"
#include "code_gen.h"

#ifndef PARSE_H
#define PARSE_H
int curr_tok;
char *iconst;
int lbls;

extern int chk_decl_flag;
extern int print_ast_flag;
extern int gen_code_flag;

scope* globalScope;
scope* currScope;
char* scopeName;

extern int parse();

void prog();
void decl_or_func();
void var_decl();
void id_list();
ASTnode* func_defn();
void type();
int opt_formals();
int formals();
int opt_formals_();
void opt_var_decls();
ASTnode* opt_stmt_list();
ASTnode* stmt();
ASTnode* if_stmt();
ASTnode* else_stmt();
ASTnode* while_stmt();
ASTnode* return_stmt();
ASTnode* assg_stmt();
ASTnode* fn_call();
ASTnode* opt_expr_list();
ASTnode* expr_list();
ASTnode* or_expr();
ASTnode* or_expr_(ASTnode* ast);
ASTnode* and_expr();
ASTnode* and_expr_(ASTnode* ast);
ASTnode* rel_expr();
ASTnode* bool_exp();
ASTnode* arith_exp();
ASTnode* add_expr();
NodeType relop();
ASTnode* mult_expr();
ASTnode* add_expr_(ASTnode* ast);
ASTnode* unary_expr();
ASTnode* mult_expr_(ASTnode* ast);
ASTnode* base();

void match(int expected);
sym* findSym(char *funcName);
void checkArgs(int expArgs, int currArgs);
void isVar();
void manageScope(int flag);
void reloadSym(SymType type);
void instertArgs(char* scope_name, int num_args);
sym* varInScope(scope* var);
sym* getvar(char* varName);
void println();

#endif 
