#include "code_gen.h"
//Debugging macro
#define DEBUG 0  
#define DEBUG_PRINT(fmt, ...) \
    do { if (DEBUG) fprintf(stderr, fmt, ##__VA_ARGS__); } while (0)


int tempNum;
scope *funcScope;
int lblNum = 0;

void codeGen_fncall(ASTnode *fn){
    DEBUG_PRINT("FUNC_CALL\n");
    if (!fn || !fn->name) {
        DEBUG_PRINT("ERROR: Null function node or name\n");
        return;
    }
    DEBUG_PRINT("Called function: %s\n", func_call_callee(fn));
    if(fn->left != NULL){
        DEBUG_PRINT("Arguments detected, beginning parameter code generation\n");
        codeGen_exprList(fn->left);
        DEBUG_PRINT("Parameter code generation complete\n");
        fn->code_hd = fn->left->code_hd;
        fn->code_tl = fn->left->code_tl;
        DEBUG_PRINT("Appending CALL quad for function '%s' with %d params\n",
                    fn->name->name, fn->name->params);
        fn->code_tl->next = newinstr(op_CALL, fn->name, NULL, fn->name->params, 0, NULL,NULL);
        fn->code_tl = fn->code_tl->next;
        DEBUG_PRINT("CALL quad appended successfully\n");
    } else {
        DEBUG_PRINT("No arguments found, generating direct CALL quad\n");
        fn->code_hd = newinstr(op_CALL, fn->name, NULL, fn->name->params, 0, NULL,NULL);
        fn->code_tl = fn->code_hd;
        DEBUG_PRINT("CALL quad for function '%s' created with no arguments\n", fn->name->name);
    }
    DEBUG_PRINT("Function call code generation complete for: %s\n", fn->name->name);
}

void codeGen_exprList(ASTnode *exp_list) {
    if (exp_list == NULL) {
        DEBUG_PRINT("[EXPR_LIST] Received NULL expression list\n");
        return;
    }
    DEBUG_PRINT("[EXPR_LIST] Processing expression node\n");
    ASTnode *curr = exp_list;
    DEBUG_PRINT("[EXPR_LIST] Generating PARAM for left expression\n");
    codeGen_param(curr->left);
    curr->code_hd = curr->left->code_hd;
    curr->code_tl = curr->left->code_tl;
    DEBUG_PRINT("[EXPR_LIST] Moving to right sublist (if any)\n");
    codeGen_exprList(curr->right);
    if (curr->right != NULL) {
        DEBUG_PRINT("[EXPR_LIST] Appending right expression list code\n");
        curr->code_tl->next = curr->right->code_hd;
        curr->code_tl = curr->right->code_tl;
    }
    DEBUG_PRINT("[EXPR_LIST] Expression list processing complete\n");
}

void codeGen_param(ASTnode *p) {
    DEBUG_PRINT("[PARAM] Processing parameter node\n");
    if (p->ntype == IDENTIFIER) {
        DEBUG_PRINT("[PARAM] Identifier param: %s\n", p->name->name);
        p->code_hd = newinstr(op_PARAM, p->name, NULL, 0, 0, NULL, NULL);
        p->code_tl = p->code_hd;
    } else if (p->ntype == INTCONST) {
        DEBUG_PRINT("[PARAM] Integer constant param: %d\n", p->num);
        p->code_hd = newinstr(op_PARAM, NULL, NULL, p->num, 0, NULL, NULL);
        p->code_tl = p->code_hd;
    } else {
        DEBUG_PRINT("[PARAM] Complex expression param, generating code first\n");
        codeGen_expr(p);
        DEBUG_PRINT("[PARAM] Attaching PARAM for expression result\n");
        p->code_tl->next = newinstr(op_PARAM, p->code_tl->dest, NULL, 0, 0, NULL, NULL);
        p->code_tl = p->code_tl->next;
    }
    DEBUG_PRINT("[PARAM] Done processing param node\n");
}

void codeGen_assgStmt(ASTnode *a) {
    DEBUG_PRINT("[ASSIGN] Generating assignment statement\n");
    DEBUG_PRINT("[ASSIGN] Processing right-hand side expression\n");
    codeGen_expr(a->left);
    a->code_hd = a->left->code_hd;
    a->code_tl = a->left->code_tl;
    DEBUG_PRINT("[ASSIGN] Assigning result to: %s\n", a->name->name);
    a->code_tl->next = newinstr(op_MOV, a->code_tl->dest, NULL, 0, 0, a->name, NULL);
    a->code_tl = a->code_tl->next;
    DEBUG_PRINT("[ASSIGN] Assignment complete\n");
}

void codeGen_expr(ASTnode *a) {
    ASTnode* E1;
    ASTnode* E2;

    DEBUG_PRINT("[EXPR] Processing expression node type: %d\n", a->ntype);

    switch (a->ntype) {
        case ADD:
            DEBUG_PRINT("[EXPR] ADD operation\n");
            codeGen_expr(a->left);
            E1 = a->left;

            codeGen_expr(a->right);
            E2 = a->right;

            E1->code_tl->next = E2->code_hd;
            DEBUG_PRINT("[EXPR] Generating ADD instruction\n");
            Quad* AADDInstr = newinstr(op_ADD, E1->code_tl->dest, E2->code_tl->dest, 0, 0, newtemp(), NULL);
            E2->code_tl->next = AADDInstr;
            a->code_hd = E1->code_hd;
            a->code_tl = AADDInstr;
            break;
        case SUB:
            DEBUG_PRINT("[EXPR] SUB operation\n");
            codeGen_expr(a->left);
            E1 = a->left;
            codeGen_expr(a->right);
            E2 = a->right;
            E1->code_tl->next = E2->code_hd;
            DEBUG_PRINT("[EXPR] Generating ASUB instruction\n");
            Quad* ASUBInstr = newinstr(op_SUB, E1->code_tl->dest, E2->code_tl->dest, 0, 0, newtemp(), NULL);
            E2->code_tl->next = ASUBInstr;
            a->code_hd = E1->code_hd;
            a->code_tl = ASUBInstr;
            break;
        case MUL:
            DEBUG_PRINT("[EXPR] MUL operation\n");
            codeGen_expr(a->left);
            E1 = a->left;
            codeGen_expr(a->right);
            E2 = a->right;
            E1->code_tl->next = E2->code_hd;
            DEBUG_PRINT("[EXPR] Generating AMUL instruction\n");
            Quad* AMULInstr = newinstr(op_MUL, E1->code_tl->dest, E2->code_tl->dest, 0, 0, newtemp(), NULL);
            E2->code_tl->next = AMULInstr;
            a->code_hd = E1->code_hd;
            a->code_tl = AMULInstr;
            break;
        case DIV:
            DEBUG_PRINT("[EXPR] DIV operation\n");
            codeGen_expr(a->left);
            E1 = a->left;
            codeGen_expr(a->right);
            E2 = a->right;
            E1->code_tl->next = E2->code_hd;
            DEBUG_PRINT("[EXPR] Generating ADIV instruction\n");
            Quad* ADIVInstr = newinstr(op_DIV, E1->code_tl->dest, E2->code_tl->dest, 0, 0, newtemp(), NULL);
            E2->code_tl->next = ADIVInstr;
            a->code_hd = E1->code_hd;
            a->code_tl = ADIVInstr;
            break;
        case UMINUS:
            DEBUG_PRINT("[EXPR] UMINUS operation\n");
            codeGen_expr(a->left);
            E1 = a->left;
            a->code_hd = E1->code_hd;
            a->code_tl = E1->code_tl;
            DEBUG_PRINT("[EXPR] Generating AMINUS instruction\n");
            Quad* opInstruction = newinstr(op_MIN, E1->code_tl->dest, NULL, 0, 0, newtemp(), NULL);
            a->code_tl->next = opInstruction;
            a->code_tl = opInstruction;
            break;
        case IDENTIFIER:
            DEBUG_PRINT("[EXPR] IDENTIFIER: %s\n", a->name->name);
            a->code_hd = newinstr(op_MOV, a->name, NULL, 0, 0, newtemp(), NULL);
            a->code_tl = a->code_hd;
            break;
        case INTCONST:
            DEBUG_PRINT("[EXPR] INTCONST: %d\n", a->num);
            a->code_hd = newinstr(op_MOV, NULL, NULL, a->num, 0, newtemp(), NULL);
            a->code_tl = a->code_hd;
            break;
        case FUNC_CALL:
            DEBUG_PRINT("[EXPR] FUNC_CALL\n");
            codeGen_fncall(a);
            Quad* retInstr = newinstr(GET_RET, NULL, NULL, 0, 0, newtemp(), NULL);
            a->code_tl->next = retInstr;
            a->code_tl = retInstr;
            break;
        default:
            DEBUG_PRINT("[EXPR] Unrecognized expression type: %d\n", a->ntype);
            break;
    }
}

void codeGen_if(ASTnode *i){
    DEBUG_PRINT("[IF] Starting code generation for if-statement\n");

    Quad *lthen = newlabel();
    Quad *lelse = newlabel();
    Quad *after = newlabel();

    DEBUG_PRINT("[IF] Generating condition code\n");
    codeGen_bool(i->left, lthen->lbl, lelse->lbl);

    
    i->code_hd = i->left->code_hd;
    i->code_tl = i->left->code_tl;

    
    DEBUG_PRINT("[IF] Adding then-label: %s\n", lthen->lbl);
    i->code_tl->next = lthen;
    i->code_tl = i->code_tl->next;

    
    if (i->right) {
        DEBUG_PRINT("[IF] Generating code for then-branch\n");
        codeGen_stmt(i->right);
        i->code_tl->next = i->right->code_hd;
        i->code_tl = i->right->code_tl;
    }

    
    DEBUG_PRINT("[IF] Adding GOTO to after-label: %s\n", after->lbl);
    i->code_tl->next = newinstr(op_GOTO, NULL, NULL, 0, 0, NULL, after->lbl);
    i->code_tl = i->code_tl->next;

    
    DEBUG_PRINT("[IF] Adding else-label: %s\n", lelse->lbl);
    i->code_tl->next = lelse;
    i->code_tl = i->code_tl->next;

    
    if (i->next) {
        DEBUG_PRINT("[IF] Generating code for else-branch\n");
        codeGen_stmt(i->next);
        i->code_tl->next = i->next->code_hd;
        i->code_tl = i->next->code_tl;
    }

    
    DEBUG_PRINT("[IF] Adding after-label: %s\n", after->lbl);
    i->code_tl->next = after;
    i->code_tl = i->code_tl->next;

    DEBUG_PRINT("[IF] Finished code generation for if-statement\n");
}


void codeGen_bool(ASTnode *b, char *tDst, char *fDst){
    DEBUG_PRINT("[BOOL] Generating code for boolean expression (type: %d)\n", b->ntype);

    if (b->ntype == AND) {
        DEBUG_PRINT("[BOOL] Detected logical AND\n");
        Quad *L1 = newlabel();
        DEBUG_PRINT("[BOOL] Created intermediate label: %s\n", L1->lbl);

        DEBUG_PRINT("[BOOL] Generating code for left operand of AND\n");
        codeGen_bool(b->left, L1->lbl, fDst);
        ASTnode* B1 = b->left;

        DEBUG_PRINT("[BOOL] Generating code for right operand of AND\n");
        codeGen_bool(b->right, tDst, fDst);
        ASTnode* B2 = b->right;

        B1->code_tl->next = L1;
        L1->next = B2->code_hd;

        b->code_hd = B1->code_hd;
        b->code_tl = B2->code_tl;

    } else if (b->ntype == OR) {
        DEBUG_PRINT("[BOOL] Detected logical OR\n");
        Quad *L1 = newlabel();
        DEBUG_PRINT("[BOOL] Created intermediate label: %s\n", L1->lbl);

        DEBUG_PRINT("[BOOL] Generating code for left operand of OR\n");
        codeGen_bool(b->left, tDst, L1->lbl);
        ASTnode* B1 = b->left;

        DEBUG_PRINT("[BOOL] Generating code for right operand of OR\n");
        codeGen_bool(b->right, tDst, fDst);
        ASTnode* B2 = b->right;

        B1->code_tl->next = L1;
        L1->next = B2->code_hd;

        b->code_hd = B1->code_hd;
        b->code_tl = B2->code_tl;

    } else {
        DEBUG_PRINT("[BOOL] Detected relational boolean expression\n");

        codeGen_expr(b->left);
        ASTnode* E1 = b->left;
        DEBUG_PRINT("[BOOL] Left operand expression evaluated\n");

        codeGen_expr(b->right);
        ASTnode* E2 = b->right;
        DEBUG_PRINT("[BOOL] Right operand expression evaluated\n");

        Quad *relopInstr = codeGen_relop(b->ntype,tDst);
        relopInstr->src1 = E1->code_tl->dest;
        relopInstr->src2 = E2->code_tl->dest;

        DEBUG_PRINT("[BOOL] Created relop instruction to jump to: %s\n", tDst);

        E1->code_tl->next = E2->code_hd;
        E2->code_tl->next = relopInstr;

        relopInstr->next = newinstr(op_GOTO, NULL, NULL, 0, 0, NULL, fDst);
        DEBUG_PRINT("[BOOL] Appended fallback GOTO to: %s\n", fDst);

        b->code_hd = E1->code_hd;
        b->code_tl = relopInstr->next;
    }

    DEBUG_PRINT("[BOOL] Finished generating code for boolean expression\n");
}

Quad *codeGen_relop(NodeType type, char* lbl){
    DEBUG_PRINT("[RELOP] Generating relational op to jump to label: %s (type: %d)\n", lbl, type);
    switch (type) {
        case EQ:
            DEBUG_PRINT("[RELOP] Operator: EQ (==)\n");
            return newinstr(op_EQ, NULL, NULL, 0, 0, NULL, lbl);
        case NE:
            DEBUG_PRINT("[RELOP] Operator: NE (!=)\n");
            return newinstr(op_NE, NULL, NULL, 0, 0, NULL, lbl);
        case LE:
            DEBUG_PRINT("[RELOP] Operator: LE (<=)\n");
            return newinstr(op_LE, NULL, NULL, 0, 0, NULL, lbl);
        case LT:
            DEBUG_PRINT("[RELOP] Operator: LT (<)\n");
            return newinstr(op_LT, NULL, NULL, 0, 0, NULL, lbl);
        case GE:
            DEBUG_PRINT("[RELOP] Operator: GE (>=)\n");
            return newinstr(op_GE, NULL, NULL, 0, 0, NULL, lbl);
        case GT:
            DEBUG_PRINT("[RELOP] Operator: GT (>)\n");
            return newinstr(op_GT, NULL, NULL, 0, 0, NULL, lbl);
        default:
            DEBUG_PRINT("[RELOP] Unknown relational operator type: %d\n", type);
            return NULL;
    }
}

void codeGen_ret(ASTnode *r){
    DEBUG_PRINT("[RETURN] Generating code for return statement\n");

    if(r->left != NULL){
        DEBUG_PRINT("[RETURN] Return has an expression\n");
        codeGen_expr(r->left);

        ASTnode *E = r->left;
        r->code_hd = E->code_hd;

        DEBUG_PRINT("[RETURN] Appending RET instruction with return value\n");
        E->code_tl->next = newinstr(op_RET, E->code_tl->dest, NULL, 0, 0, NULL, NULL);
        r->code_tl = E->code_tl->next;
    }
    else{
        DEBUG_PRINT("[RETURN] Return has no expression\n");
        r->code_hd = newinstr(op_RET, NULL, NULL, 0, 0, NULL, NULL);
        r->code_tl = r->code_hd;
    }

    DEBUG_PRINT("[RETURN] Appending LEAVE instruction\n");
    r->code_tl->next = newinstr(op_LEAVE, NULL, NULL, 0, 0, NULL, NULL);
    r->code_tl = r->code_tl->next;
}

void codeGen_while(ASTnode *w){
    DEBUG_PRINT("[WHILE] Generating while loop\n");

    Quad *top = newlabel();
    Quad *eval = newlabel();
    Quad *after = newlabel();

    DEBUG_PRINT("[WHILE] Labels: top=%s, eval=%s, after=%s\n", top->lbl, eval->lbl, after->lbl);

    
    DEBUG_PRINT("[WHILE] Generating code for loop condition\n");
    codeGen_bool(w->left, top->lbl, after->lbl);

    
    DEBUG_PRINT("[WHILE] Emitting GOTO to eval label: %s\n", eval->lbl);
    w->code_hd = newinstr(op_GOTO, NULL, NULL, 0, 0, NULL, eval->lbl);
    w->code_tl = w->code_hd;

    
    DEBUG_PRINT("[WHILE] Inserting loop top label: %s\n", top->lbl);
    w->code_tl->next = top;
    w->code_tl = w->code_tl->next;

    
    if (w->right) {
        DEBUG_PRINT("[WHILE] Generating code for loop body\n");
        codeGen_stmt(w->right);
        w->code_tl->next = w->right->code_hd;
        w->code_tl = w->right->code_tl;
    }

    
    DEBUG_PRINT("[WHILE] Inserting eval label: %s\n", eval->lbl);
    w->code_tl->next = eval;
    w->code_tl = w->code_tl->next;

    DEBUG_PRINT("[WHILE] Appending condition code\n");
    w->code_tl->next = w->left->code_hd;
    w->code_tl = w->left->code_tl;

    
    DEBUG_PRINT("[WHILE] Inserting after label: %s\n", after->lbl);
    w->code_tl->next = after;
    w->code_tl = w->code_tl->next;

    DEBUG_PRINT("[WHILE] Done generating while loop\n");
}



void codeGen_stmt(ASTnode *s){
    if(s == NULL){
        DEBUG_PRINT("[STMT] Null statement node. Skipping.\n");
        return;
    }
    DEBUG_PRINT("[STMT] Generating code for statement type: %d\n", s->ntype);

    switch (s->ntype) {
        case FUNC_CALL:
            DEBUG_PRINT("[STMT] Function call\n");
            codeGen_fncall(s);
            break;
            
        case STMT_LIST:
            DEBUG_PRINT("[STMT] Statement list\n");
            codeGen_stmtList(s);
            break;
            
        case ASSG:
            DEBUG_PRINT("[STMT] Assignment statement\n");
            codeGen_assgStmt(s);
            break;
            
        case IF:
            DEBUG_PRINT("[STMT] If statement\n");
            codeGen_if(s);
            break;
            
        case WHILE:
            DEBUG_PRINT("[STMT] While loop\n");
            codeGen_while(s);
            break;
            
        case RETURN:
            DEBUG_PRINT("[STMT] Return statement\n");
            codeGen_ret(s);
            break;
            
        default:
            DEBUG_PRINT("[STMT] Unknown statement type: %d\n", s->ntype);
            break;
    }
}

void codeGen_stmtList(ASTnode *StmtListHead){
    if(StmtListHead == NULL){
        DEBUG_PRINT("[STMT_LIST] Empty statement list node. Skipping.\n");
        return;
    }

    DEBUG_PRINT("[STMT_LIST] Processing statement list node.\n");

    ASTnode *list = StmtListHead;
    ASTnode *curr = stmt_list_head(list);

    DEBUG_PRINT("[STMT_LIST] Processing head of statement list.\n");
    codeGen_stmt(curr);

    list->code_hd = curr->code_hd;
    list->code_tl = curr->code_tl;

    if(list->right != NULL){
        DEBUG_PRINT("[STMT_LIST] Processing rest of statement list.\n");
        codeGen_stmtList(stmt_list_rest(list));
        list->code_tl->next = list->right->code_hd;
        list->code_tl = list->right->code_tl;
    } else {
        DEBUG_PRINT("[STMT_LIST] No more statements in list.\n");
    }
}

void codeGen_funcdef(ASTnode *f){
    DEBUG_PRINT("[FUNC_DEF] Generating code for function definition: %s\n", f->name->name);

    
    DEBUG_PRINT("[FUNC_DEF] Emitting ENTER instruction\n");
    f->code_hd = newinstr(op_ENTER, f->name, NULL, 0, 0, NULL, NULL);
    f->code_tl = f->code_hd;

    ASTnode *list = func_def_body(f);
    if (list != NULL) {
        DEBUG_PRINT("[FUNC_DEF] Found function body. Generating code for body.\n");
        codeGen_stmtList(list);

        
        DEBUG_PRINT("[FUNC_DEF] Attaching function body code to ENTER\n");
        f->code_tl->next = f->left->code_hd;
        f->code_tl = f->left->code_tl;
    } else {
        DEBUG_PRINT("[FUNC_DEF] No body found for function\n");
    }

    
    DEBUG_PRINT("[FUNC_DEF] Emitting LEAVE instruction\n");
    f->code_tl->next = newinstr(op_LEAVE, f->name, NULL, 0, 0, NULL, NULL);
    f->code_tl = f->code_tl->next;
}
Quad* newlabel() {
    char labelBuf[32];  
    snprintf(labelBuf, sizeof(labelBuf), "__L%d", lblNum++);

    DEBUG_PRINT("[NEW_LABEL] Creating label: %s\n", labelBuf);

    return newinstr(op_LABEL, NULL, NULL, 0, 0, NULL, strdup(labelBuf));
}



Quad* newinstr(opType Quadtype, sym *src1, sym *src2, int num1, int num2, sym *dest, char *lbl) {
    Quad *newInstr = malloc(sizeof(Quad));
    newInstr->op = Quadtype;
    newInstr->src1num = num1;
    newInstr->src2num = num2;
    newInstr->src1 = src1;
    newInstr->src2 = src2;
    newInstr->dest = dest;

    if (lbl != NULL) {
        newInstr->lbl = strdup(lbl);
        DEBUG_PRINT("[NEW_INSTR] Created instr with label: %s\n", lbl);
    } else {
        newInstr->lbl = NULL;
        DEBUG_PRINT("[NEW_INSTR] Created instr of type %d without label\n", Quadtype);
    }

    if (dest)
        DEBUG_PRINT("[NEW_INSTR] Dest: %s\n", dest->name);
    if (src1)
        DEBUG_PRINT("[NEW_INSTR] Src1: %s\n", src1->name);
    if (src2)
        DEBUG_PRINT("[NEW_INSTR] Src2: %s\n", src2->name);
    if (!src1 && num1)
        DEBUG_PRINT("[NEW_INSTR] Src1 Num: %d\n", num1);
    if (!src2 && num2)
        DEBUG_PRINT("[NEW_INSTR] Src2 Num: %d\n", num2);

    return newInstr;
}

sym* newtemp() {
    char nameBuf[32];
    snprintf(nameBuf, sizeof(nameBuf), "tmp%d", tempNum++);

    DEBUG_PRINT("[NEWTEMP] Creating temporary variable: %s\n", nameBuf);

    
    insertSymbol(&(funcScope->symbols), nameBuf, TEMP, funcScope);
    sym* ntemp = getTemp(funcScope);

    if (ntemp) {
        DEBUG_PRINT("[NEWTEMP] Temp '%s' inserted with offset: %d\n", ntemp->name, ntemp->offset);
    } else {
        DEBUG_PRINT("[NEWTEMP] Failed to retrieve newly created temp\n");
    }

    return ntemp;
}

void gen_code(ASTnode* ast, scope *fnScope){
    tempNum = 0;
    funcScope = fnScope;
    codeGen_funcdef(ast);
    int loNum = get_offsets(ast);
    gen_mips(ast->code_hd, loNum);
}

int get_offsets(ASTnode *f){
    
    int pNum = f->name->params;
    
    int pOff = pNum+1;
    int loOff = 1;
    
    
    sym *currVar = funcScope->symbols;
    DEBUG_PRINT("Total parameters: %d\n", pNum);
    if (pNum != 0) {
        while (pOff != 1) {
            currVar->offset = 4*pOff;
            DEBUG_PRINT("Param '%s' assigned offset: %d\n", currVar->name, currVar->offset);
            pOff--;
            
            currVar = currVar->next;
        }
    }
    
    while (currVar != NULL) {
        currVar->offset = -4*loOff;
        DEBUG_PRINT("Local '%s' assigned offset: %d\n", currVar->name, currVar->offset);
        loOff++;
        currVar = currVar->next;
    }
    DEBUG_PRINT("Total local variables: %d\n", loOff - 1);
    return loOff;
}