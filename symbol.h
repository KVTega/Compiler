

#ifndef SYMBOL_H
#define SYMBOL_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "scanner.h"

typedef enum {
    FUNC,
    TEMP,
    INT
}SymType;

// local variables for scopes
typedef struct Symbol{
    SymType sType;
    char* name;
    struct Symbol* next;
    int params;
    
    //used for functions points to the scope struct
    struct Scope* ptr;
    
    //offset for mem for the stack
    int offset;
    
    //for global handling
    int global;
    
}sym;


// will be used for global and local scopes
typedef struct Scope{
    
    char* name;
    sym* symbols;
    struct Scope* next;
    struct Scope* prev;
    int global;
    
}scope;

scope* initGlobal(void);

scope* pushScope(scope* currScope, char* scopeLex);

scope* popScope(scope* currScope);

int insertSymbol(sym **sym_hd, char* name, SymType type,scope* currScope);

sym *getTemp(scope *currScope);

#endif
