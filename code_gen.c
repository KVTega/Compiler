#include "code_gen.h"
// Debugging macro
#define DEBUG 0  // <-- Set to 0 to disable debug printing
#define DEBUG_PRINT(fmt, ...) \
    do { if (DEBUG) fprintf(stderr, fmt, ##__VA_ARGS__); } while (0)

void gen_mips(Quad *curr, int loNum){
    printf("\n\n");
    
    while (curr != NULL) {
        switch (curr->op) {
            case op_MOV:
                DEBUG_PRINT("#MOV:\n");
                if (curr->src1 != NULL) {
                    printf("    lw $t0,");
                    printSrc(curr->src1);
                }
                else{
                    printf("    li $t0,%d\n",curr->src1num);
                }
                printf("    sw $t0,");
                printSrc(curr->dest);
                break;
            case op_SUB:
                printf("# SUB:\n");
                printf("    lw $t0,");
                printSrc(curr->src1);
                printf("    lw $t1,");
                printSrc(curr->src2);
                printf("    sub $t2, $t0, $t1\n");
                printf("    sw $t2,");
                printSrc(curr->dest);
                break;
                
            case op_ADD:
                DEBUG_PRINT("# ADD:\n");
                printf("    lw $t0,");
                printSrc(curr->src1);
                printf("    lw $t1,");
                printSrc(curr->src2);
                printf("    add $t2, $t0, $t1\n");
                printf("    sw $t2,");
                printSrc(curr->dest);
                break;
                
            case op_DIV:
                DEBUG_PRINT("# DIV:\n");
                printf("    lw $t0,");
                printSrc(curr->src1);
                printf("    lw $t1,");
                printSrc(curr->src2);
                printf("    div $t2, $t0, $t1\n");
                printf("    sw $t2,");
                printSrc(curr->dest);
                break;
                
            case op_MUL:
                DEBUG_PRINT("# MUL:\n");
                printf("    lw $t0,");
                printSrc(curr->src1);
                printf("    lw $t1,");
                printSrc(curr->src2);
                printf("    mul $t2, $t0, $t1\n");
                printf("    sw $t2,");
                printSrc(curr->dest);
                break;
            case op_MIN:
                DEBUG_PRINT("#UMINUS:\n");
                printf("    lw $t0,");
                printSrc(curr->src1);
                printf("    neg $t1, $t0\n");
                printf("    sw $t1,");
                printSrc(curr->dest);
                break;
            case op_ENTER:
                DEBUG_PRINT("#ENTER:\n");
                printf("_%s:\n",curr->src1->name);
                printf("    la $sp, -8($sp)\n");
                printf("    sw $fp, 4($sp)\n");
                printf("    sw $ra, 0($sp)\n");
                printf("    la $fp, 0($sp)\n");
                printf("    la $sp, %d($sp)\n",((loNum-1)*-4));
                break;
                
            case op_LEAVE:
                DEBUG_PRINT("#LEAVE:\n");
                printf("    la $sp, 0($fp)\n");
                printf("    lw $ra, 0($sp)\n");
                printf("    lw $fp, 4($sp)\n");
                printf("    la $sp, 8($sp)\n");
                printf("    jr $ra\n");
                break;
                
            case op_PARAM:
                DEBUG_PRINT("#PARAM:\n");
                if(curr->src1 != NULL){
                    printf("    lw $t0,");
                    printSrc(curr->src1);
                }
                else{
                    printf("    li $t0,%d\n",curr->src1num);
                }
                printf("    la $sp, -4($sp)\n");
                printf("    sw $t0, 0($sp)\n");
                break;
                
            case op_CALL:
                DEBUG_PRINT("#CALL:\n");
                printf("    jal _%s\n",curr->src1->name);
                printf("    la $sp, %d($sp)\n",(curr->src1num*4));
                break;
                
            case op_EQ:
                DEBUG_PRINT("#IF_EQ\n");
                
                printf("    lw $t0,"); printSrc(curr->src1);
                printf("    lw $t1,"); printSrc(curr->src2);
                printf("    beq $t0,$t1,%s\n",curr->lbl);
                break;
                
            case op_NE:
                DEBUG_PRINT("#IF_NEQ\n");
                
                printf("    lw $t0,"); printSrc(curr->src1);
                printf("    lw $t1,"); printSrc(curr->src2);
                printf("    bne $t0,$t1,%s\n",curr->lbl);
                break;
                
            case op_GT:
                DEBUG_PRINT("#IF_GT\n");
                
                printf("    lw $t0,"); printSrc(curr->src1);
                printf("    lw $t1,"); printSrc(curr->src2);
                printf("    bgt $t0,$t1,%s\n",curr->lbl);
                break;
                
            case op_GE:
                DEBUG_PRINT("#IF_GTE\n");
                
                printf("    lw $t0,"); printSrc(curr->src1);
                printf("    lw $t1,"); printSrc(curr->src2);
                printf("    bge $t0,$t1,%s\n",curr->lbl);
                break;
                
            case op_LT:
                DEBUG_PRINT("#LT\n");
                
                printf("    lw $t0,"); printSrc(curr->src1);
                printf("    lw $t1,"); printSrc(curr->src2);
                printf("    blt $t0,$t1,%s\n",curr->lbl);
                break;
                
            case op_LE:
                DEBUG_PRINT("#LE\n");
                
                printf("    lw $t0,"); printSrc(curr->src1);
                printf("    lw $t1,"); printSrc(curr->src2);
                printf("    ble $t0,$t1,%s\n",curr->lbl);
                break;
                
            case op_RET:
                DEBUG_PRINT("#RETURN\n");
                if (curr->src1 == NULL) {
                    break;
                }
                printf("    lw $v0,");
                printSrc(curr->src1);
                break;
            case op_GOTO:
                printf("    j\t%s\n",curr->lbl);
                break;
                
            case op_LABEL:
                printf("%s:\n", curr->lbl);
                break;
            
            case GET_RET:
                DEBUG_PRINT("#GET RETURN\n");
                printf("    sw $v0, ");
                printSrc(curr->dest);
                
            default:
                break;
        }
        curr = curr->next;
    }
    printf("\n\n");
}

void printSrc(sym *s){
    if (s->global) {
        printf("__%s\n",s->name);
    }else{
        printf("%d($fp)\n",s->offset);
    }
}

void gen_globals(scope *globalScope){
    printf("\n\n.data\n");
    printf(".align 2\n");
    
    sym *curr = globalScope->symbols;
    while (curr != NULL) {
        if (curr->sType == INT) {
            printf("__%s: .space 4\n",curr->name);
        }
        
        curr = curr->next;
    }
    printf(".text\n");
}

void print_ln() {
    printf("\n.align 2\n"
        ".data\n"
        "_nl: .asciiz \"\\n\"\n"
        ".align 2\n"
        ".text\n"
        "# println: print out an integer followed by a newline\n"
        "_println:\n"
        "    li $v0, 1\n"
        "    lw $a0, 0($sp)\n"
        "    syscall\n"
        "    li $v0, 4\n"
        "    la $a0, _nl\n"
        "    syscall\n"
        "    jr $ra\n"
    );
}