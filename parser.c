
#include "parse.h"
#define DEBUG 1 

#define DEBUG_PRINT(fmt, ...) \
    do { if (DEBUG) fprintf(stderr, fmt, ##__VA_ARGS__); } while (0)


const char* token_to_string(int tok) {
    switch (tok) {
    case UNDEF: return "UNDEF";
    case ID: return "ID";
    case INTCON: return "INTCON";
    case LPAREN: return "LPAREN";
    case RPAREN: return "RPAREN";
    case LBRACE: return "LBRACE";
    case RBRACE: return "RBRACE";
    case COMMA: return "COMMA";
    case SEMI: return "SEMI";
    case kwINT: return "kwINT";
    case kwIF: return "kwIF";
    case kwELSE: return "kwELSE";
    case kwWHILE: return "kwWHILE";
    case kwRETURN: return "kwRETURN";
    case opASSG: return "opASSG";
    case opADD: return "opADD";
    case opSUB: return "opSUB";
    case opMUL: return "opMUL";
    case opDIV: return "opDIV";
    case opEQ: return "opEQ";
    case opNE: return "opNE";
    case opGT: return "opGT";
    case opGE: return "opGE";
    case opLT: return "opLT";
    case opLE: return "opLE";
    case opAND: return "opAND";
    case opOR: return "opOR";
    case opNOT: return "opNOT";
    case EOF: return "EOF";
    default: return "UNKNOWN";
    }
}

int parse(){
    lbls = 0;
    curr_tok = get_token();
    DEBUG_PRINT("Initializing Global\n");
    globalScope = initGlobal();
    currScope = globalScope;
    DEBUG_PRINT("Initializing print\n");
    println();
    prog();
    if(gen_code_flag){
        printf(".text\n.globl main\n"
            "main:\n"
            "    jal _main\n"
            "    li $v0, 10\n"
            "    syscall\n");
        gen_globals(globalScope);
    }
    return 0;
}

void prog(){
    DEBUG_PRINT("Prog\n");
    while(curr_tok == kwINT){
        decl_or_func();
    }
    if(curr_tok == EOF){
        return;
    }
    else{
        fprintf(stderr, "[PROG]ERROR: LINE %d\n", lineNum);
        exit(1);
    }
}

void decl_or_func(){
    type();
    match(ID);
    if(curr_tok == SEMI || curr_tok == COMMA){
        reloadSym(INT);
        var_decl();
        return;
    }
    
    if(curr_tok == LPAREN){
        ASTnode* ast = func_defn();
        if (print_ast_flag) {
            print_ast(ast);
        }
        if (gen_code_flag) {
            print_ln();
            gen_code(ast, currScope);
        }
        
        manageScope(2);
        return;
    }
    fprintf(stderr, "[D_F]ERROR: LINE %d\n", lineNum);
    exit(1);
}

void opt_var_decls(){
    while (curr_tok == kwINT) {
        type();
        match(ID);
        
        reloadSym(INT);
        var_decl();
    }
    
    return;
}

void var_decl(){
    if (curr_tok == COMMA) {
        id_list();
    }
    match(SEMI);
}

void id_list(){
    while(curr_tok == COMMA){
        match(COMMA);
        match(ID);
        reloadSym(INT);
    }
    return;
}

ASTnode* func_defn(){
    manageScope(1);
    ASTnode* ast = malloc(sizeof(ASTnode));
    ast->ntype = FUNC_DEF;
    sym* symFunc = findSym(scopeName);
    ast->name = symFunc;
    
    match(LPAREN);
    
    int params = opt_formals();
    instertArgs(currScope->name,params);
    
    match(RPAREN);
    match(LBRACE);
    opt_var_decls();
    ASTnode* stmt_head = opt_stmt_list();
    match(RBRACE);
    ast->left = stmt_head;
    
    return ast;
}

void type(){
    match(kwINT);
}

int opt_formals(){
    if(curr_tok == kwINT){
        return formals();
    }
    return 0;
}

int formals(){
    type();
    match(ID);
    
    reloadSym(INT);
    return opt_formals_();
}

int opt_formals_(){
    int params = 1;
    while (curr_tok == COMMA) {
        match(COMMA);
        type();
        match(ID);
        
        reloadSym(INT);
        params++;
    }
    
    return params;
}


ASTnode* opt_stmt_list(){
    
    ASTnode* ast_hd = NULL;
    ASTnode* curr = NULL;
    
    
    while(curr_tok == ID || curr_tok == kwWHILE || curr_tok == kwIF || curr_tok == kwRETURN || curr_tok == LBRACE || curr_tok == SEMI){
        ASTnode* aststmt = stmt();
        if (ast_hd == NULL) {
            ast_hd = malloc(sizeof(ASTnode));
            ast_hd->ntype = STMT_LIST;
            ast_hd->left = aststmt;
            curr = ast_hd;
        }
        else{
            curr->right = malloc(sizeof(ASTnode));
            curr = curr->right;
            curr->ntype = STMT_LIST;
            curr->left = aststmt;
        }
    }
    return ast_hd;
}

ASTnode* stmt(){
    
    ASTnode* ast = NULL;
    switch (curr_tok) {
        case ID:
            match(ID);
            if (curr_tok == LPAREN) {
                ast = fn_call();
                match(SEMI);
            }
            else{
                ast = assg_stmt();
            }
            break;
            
        case kwWHILE:
            ast = while_stmt();
            break;
            
        case kwIF:
            ast = if_stmt();
            break;
            
        case kwRETURN:
            ast = return_stmt();
            break;
            
        case LBRACE:
            match(LBRACE);
            ast = opt_stmt_list();
            match(RBRACE);
            break;
            
        default:
            match(SEMI);
            break;
    }
    return ast;
}

ASTnode* if_stmt(){
    ASTnode* ast = malloc(sizeof(ASTnode));
    ast->ntype = IF;
    
    match(kwIF);
    match(LPAREN);
    ast->left = bool_exp();
    match(RPAREN);
    ast->right = stmt();
    ast->next = else_stmt();
    return ast;
}

ASTnode* else_stmt(){
    
    ASTnode* ast = NULL;
    if(curr_tok == kwELSE){
        match(kwELSE);
        ast = stmt();
    }
    
    return ast;
}

ASTnode* while_stmt(){
    ASTnode* ast = malloc(sizeof(ASTnode));
    ast->ntype = WHILE;
    
    match(kwWHILE);
    match(LPAREN);
    ast->left = bool_exp();
    match(RPAREN);
    ast->right = stmt();
    
    return ast;
}

ASTnode* return_stmt(){
    match(kwRETURN);
    
    ASTnode* ast = malloc(sizeof(ASTnode));
    ast->ntype = RETURN;
    if(curr_tok == INTCON || curr_tok == ID || curr_tok == LPAREN || curr_tok == opSUB){
        ast->left = arith_exp();
    }
    match(SEMI);
    return ast;
}

ASTnode* assg_stmt(){

    isVar();
    ASTnode* ast = malloc(sizeof(ASTnode));
    ast->ntype = ASSG;
    
    
    ast->name = getvar(scopeName);
    
    match(opASSG);
    ast->left = arith_exp();
    match(SEMI);
    return ast;
}

ASTnode* fn_call(){
    sym* func = findSym(scopeName);
    ASTnode* caller = malloc(sizeof(ASTnode));
    
    if(varInScope(currScope) != NULL && chk_decl_flag){
        fprintf(stderr, "%s() called a var in scope - SEMANTIC",scopeName);
        fprintf(stderr, "ERROR: LINE %d\n", lineNum);
        exit(1);
    }
    
    caller->ntype = FUNC_CALL;
    caller->name = func;
    
    match(LPAREN);
    caller->left = opt_expr_list();
    match(RPAREN);
    
    if(chk_decl_flag){
        int numArgs = 0;
        if(caller->left != NULL){
            ASTnode* curr = caller->left;
            while (curr!=NULL) {
                numArgs++;
                curr = curr->right;
            }
        }
        checkArgs(func->params, numArgs);
    }
    return caller;
}

ASTnode* opt_expr_list(){
    ASTnode* ast = NULL;
    if (curr_tok == LPAREN || curr_tok == opSUB ||curr_tok == ID || curr_tok == INTCON) {
        ast = expr_list();
    }
    return ast;
}

ASTnode* expr_list(){
    
    ASTnode* ast = malloc(sizeof(ASTnode));
    ASTnode* curr = ast;
    
    curr->ntype = EXPR_LIST;
    curr->left = arith_exp();
    
    while (curr_tok == COMMA) {
        match(COMMA);
        curr->right = malloc(sizeof(ASTnode));
        curr->right->ntype = EXPR_LIST;
        curr = curr->right;
        curr->left = arith_exp();
    }
    return ast;
}
ASTnode* bool_exp(){
    return or_expr();
}

ASTnode* or_expr(){
    ASTnode *left = and_expr();
    return or_expr_(left);
}

ASTnode* or_expr_(ASTnode* left){
    while (curr_tok == opOR) {
        ASTnode* newNode = malloc(sizeof(ASTnode));
        newNode->left = left;
        match(opOR);
        newNode->ntype = OR;
        
        newNode->right = and_expr();
        left = newNode;
    }
    return left;
}

ASTnode* and_expr(){
    ASTnode* left = rel_expr();
    return and_expr_(left);
}

ASTnode* and_expr_(ASTnode* left){
    while (curr_tok == opAND) {
        ASTnode* newNode = malloc(sizeof(ASTnode));
        newNode->left = left;
        match(opAND);
        newNode->ntype = AND;
        newNode->right = rel_expr();
        left = newNode;
    }
    return left;
}

ASTnode* rel_expr(){

    ASTnode *left = malloc(sizeof(ASTnode));
    left = arith_exp();
    
    if(curr_tok == opEQ || curr_tok == opNE || curr_tok == opLE || curr_tok == opLT || curr_tok == opGE || curr_tok == opGT){
        NodeType opType = relop();
        ASTnode *boolNode = malloc(sizeof(ASTnode));
        boolNode->ntype = opType;
        boolNode->left = left;
        boolNode->right = arith_exp();
        return boolNode;
    }
    return left;
}

NodeType relop(){
    NodeType astType = DUMMY;
    switch (curr_tok) {
        case opEQ:
            match(opEQ);
            astType = EQ;
            break;
        case opNE:
            match(opNE);
            astType = NE;
            break;
        case opLE:
            match(opLE);
            astType = LE;
            break;
        case opLT:
            match(opLT);
            astType = LT;
            break;
        case opGE:
            match(opGE);
            astType = GE;
            break;
        case opGT:
            match(opGT);
            astType = GT;
            break;
        default:
            fprintf(stderr, "[relop]ERROR: LINE %d\n", lineNum);
    exit(1);
    }
    return astType;
}


ASTnode* arith_exp(){
    return add_expr();
}

ASTnode* add_expr(){
    
    ASTnode *left = mult_expr();
    return add_expr_(left);
}

ASTnode* add_expr_(ASTnode *left){
    
    while (curr_tok == opADD || curr_tok==opSUB) {
        ASTnode *newNode = malloc(sizeof(ASTnode));
        newNode->left = left;
        if(curr_tok == opADD){
            match(opADD);
            newNode->ntype = ADD;
        }
        else if (curr_tok == opSUB){
            match(opSUB);
            newNode->ntype = SUB;
        }
        newNode->right = mult_expr();
        left = newNode;
    }
    return left;
}

ASTnode* mult_expr(){
    ASTnode *left = unary_expr();
    return mult_expr_(left);
    return NULL;
}

ASTnode* mult_expr_(ASTnode* left){
    while (curr_tok == opMUL || curr_tok==opDIV) {
        ASTnode* newNode = malloc(sizeof(ASTnode));
        newNode->left = left;
        if(curr_tok == opMUL){
            match(opMUL);
            newNode->ntype = MUL;
        }
        else if (curr_tok == opDIV){
            match(opDIV);
            newNode->ntype = DIV;
        }
        newNode->right = unary_expr();
        left = newNode;
    }
    return left;
}

ASTnode* unary_expr(){
    
    if (curr_tok == opSUB) {
        match(opSUB);
        ASTnode* node = malloc(sizeof(ASTnode));
        node->ntype = UMINUS;
        node->left = unary_expr();
        return node;
    }
    return base();
}

ASTnode* base(){
    ASTnode *ast = NULL;
    if (curr_tok == ID) {
        match(ID);
        if (curr_tok == LPAREN) {
            ast = fn_call();
        }
        else{
            ast = malloc(sizeof(ASTnode));
            isVar();
            ast->ntype = IDENTIFIER;
            ast->name = getvar(scopeName);
            ast->left = ast->right = ast->next = NULL;
        }
    }
    else if (curr_tok == INTCON){
        match(INTCON);
        ast = malloc(sizeof(ASTnode));
        ast->num = Ival;
        ast->ntype = INTCONST;
        ast->left = ast->right = ast->next = NULL;
    }
    else if(curr_tok == LPAREN){
        match(LPAREN);
        ast = arith_exp();
        match(RPAREN);
    }
    else{
        fprintf(stderr, "[BASE]ERROR: LINE %d\n", lineNum);
    exit(1);
    }
    return ast;
}

void match(int expected) {
    if (curr_tok == expected) {
        DEBUG_PRINT("Matching curr_tok %s with expected %s\n", token_to_string(curr_tok), token_to_string(expected));
        curr_tok = get_token();
        if (curr_tok != EOF) {
            scopeName = strdup(lexeme);
        }
    }
    else {
        DEBUG_PRINT("Tried to match %s with expected %s\n", token_to_string(curr_tok), token_to_string(expected));
        printf("ERROR LINE %d: was expecting %s but got %s\n", lineNum, token_to_string(expected), token_to_string(curr_tok));
        exit(1);
    }
}

void manageScope(int flag){
    if (flag == 1) {
        int err = insertSymbol(&(globalScope->symbols), scopeName, FUNC,currScope);
        if(chk_decl_flag && err){
            fprintf(stderr, "%s() already declared - SEMANTIC ",scopeName);
            fprintf(stderr, "ERROR: LINE %d\n", lineNum);
            exit(1);
        }
        currScope = pushScope(currScope, scopeName);
        sym* funcNode = findSym(scopeName);
        funcNode->ptr = currScope;
    }
    if (flag == 2) {
        currScope = popScope(currScope);
    }
}

void isVar(){
    int var = 0;
    if ((varInScope(globalScope) != NULL) || (varInScope(currScope)!= NULL)) {
        var = 1;
    }
    if (chk_decl_flag && !var) {
        fprintf(stderr, "ERROR: LINE %d\n", lineNum);
    exit(1);
    }
}

sym* varInScope(scope* scptr){
    sym* curr = scptr->symbols;
    while(curr){
        if((strcmp(curr->name, scopeName) == 0) &&
            curr->sType == INT){
            return curr;
        }
        curr = curr->next;
    }
    return NULL;
}

sym* findSym(char* funcName){
    
    int found = 0;
    sym* curr = globalScope->symbols;
    while (curr != NULL) {
        if(strcmp(curr->name, funcName) == 0 && curr->sType == FUNC){
            found = 1;
            break;
        }
        curr = curr->next;
    }
    
    if(chk_decl_flag && !found){
        fprintf(stderr, "%s() not a function - SEMANTIC ",scopeName);
        fprintf(stderr, "ERROR: LINE %d\n", lineNum);
    exit(1);
    }
    return curr;
}

void checkArgs(int expArgs, int numArgs){
    
    if (!chk_decl_flag) { return; }
    if (expArgs != numArgs) {
        fprintf(stderr, "argument expected %d arguments called %d - SEMANTIC",numArgs, expArgs);
        fprintf(stderr, "ERROR: LINE %d\n", lineNum);
    exit(1);
    }
}

void reloadSym(SymType sType){
    int err = insertSymbol(&(currScope->symbols), scopeName, sType,currScope);
    printf("%s\n",scopeName);
    if(chk_decl_flag && err){
        fprintf(stderr, "[RSym]ERROR: LINE %d\n", lineNum);
    exit(1);
    }
}

void instertArgs(char* name, int params){
    
    sym* curr = findSym(name);
    curr->params = params;
}

sym* getvar(char* varName){
    sym* var;
    var = varInScope(currScope);
    if (var) {
        return var;
    }
    else{
        return varInScope(globalScope);
    }
}

void println(){
    scopeName = strdup("println");
    manageScope(1);
    instertArgs(currScope->name,1);
    scopeName = strdup("x");
    reloadSym(INT);
    manageScope(2);
}

