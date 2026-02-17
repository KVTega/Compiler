#ifndef TAC_H
#define TAC_H
#include "symbol.h"

typedef enum {
    op_MOV = 0,     // 0: Move src1 into dest (e.g., dest = src1 or dest = immediate)
    op_ADD,         // 1: dest = src1 + src2
    op_SUB,         // 2: dest = src1 - src2
    op_DIV,         // 3: dest = src1 / src2
    op_MUL,         // 4: dest = src1 * src2
    op_MIN,         // 5: dest = -src1 (unary minus)

    op_ENTER,       // 6: Function prologue (setup stack frame)
    op_PARAM,       // 7: Push a parameter before a function call
    op_CALL,        // 8: Call a function (jal _funcName)
    op_LEAVE,       // 9: Function epilogue (restore stack & return)

    op_LABEL,       //10: Define a label in code (used for jumps)
    op_GT,          //11: If src1 > src2 then jump to label
    op_GE,          //12: If src1 >= src2 then jump to label
    op_LT,          //13: If src1 < src2 then jump to label
    op_LE,          //14: If src1 <= src2 then jump to label
    op_EQ,          //15: If src1 == src2 then jump to label
    op_NE,          //16: If src1 != src2 then jump to label
    op_GOTO,        //17: Unconditional jump to label
    op_RET,         //18: Return from function with optional value
    GET_RET         //19: Retrieve return value into dest
} opType;

typedef struct Quad{
    opType op;
    sym* src1;
    sym* src2;
    sym* dest;
    struct Quad *next;
    int src1num;
    int src2num;
    char* lbl;
}Quad;


#endif
