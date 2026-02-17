#include "symbol.h"
#define DEBUG 1 

#define DEBUG_PRINT(fmt, ...) \
    do { if (DEBUG) fprintf(stderr, fmt, ##__VA_ARGS__); } while (0)


scope* initGlobal(){
    scope* globalScope = malloc(sizeof(scope));
    globalScope->name = NULL;
    globalScope->symbols = NULL;
    globalScope->next = NULL;
    globalScope->prev = NULL;
    globalScope->global = 1;
    DEBUG_PRINT("Global\n");
    return globalScope;
}

scope* pushScope(scope* currScope, char* name){
    
    scope* newScope = malloc(sizeof(scope));
    
    newScope->name = strdup(name);
    newScope->symbols = NULL;
    newScope->next = NULL;
    newScope->prev = currScope;
    newScope->global = 0;
    
    return newScope;
}

scope* popScope(scope* currScope){
    scope* prev = currScope->prev;
    prev->next = NULL;
    return prev;
}
int insertSymbol(sym** head, char* name, SymType sType, scope *currScope) {
    // If list is empty, insert at head
    if (*head == NULL) {
        *head = malloc(sizeof(sym));
        (*head)->sType = sType;
        (*head)->params = 0;
        (*head)->name = strdup(name);
        (*head)->global = currScope->global ? 1 : 0;
        (*head)->next = NULL;
        (*head)->ptr = (sType == FUNC) ? currScope : NULL;
        return 0;
    }

    // Traverse to end, checking for duplicates
    sym* curr = *head;
    while (curr->next != NULL) {
        if (strcmp(curr->name, name) == 0) {
            DEBUG_PRINT("ALR exists %s\n",curr->name);
            return 1; // Duplicate found
        }
        curr = curr->next;
    }

    // Final check for duplicate at the last node
    if (strcmp(curr->name, name) == 0) {
        DEBUG_PRINT("ALR exists %s\n",curr->name);
        return 1;
    }

    // Insert new symbol at the end
    DEBUG_PRINT("Inserting %s\n",curr->name);
    sym* newSym = malloc(sizeof(sym));
    newSym->sType = sType;
    newSym->params = 0;
    newSym->name = strdup(name);
    newSym->global = currScope->global ? 1 : 0;
    newSym->next = NULL;
    newSym->ptr = (sType == FUNC) ? currScope : NULL;

    curr->next = newSym;
    return 0;
}


sym *getTemp(scope *currScope){
    
    sym *curr = currScope->symbols;
    
    while (curr->next != NULL) {
        curr = curr->next;
    }
    return curr;
}
