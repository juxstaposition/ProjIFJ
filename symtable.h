/***
 * Projekt IFJ2018
 * @brief	Header file for symtable
 * @author: Michal Vanka, xvanka00
 *
 * Defines structure of symtable and prototypes of functions with symbtable.
 */

#ifndef PROJIFJ_SYMTABLE_H
#define PROJIFJ_SYMTABLE_H

#include "main.h"
#include <stdarg.h>
#include <stdbool.h>
#include <string.h>

typedef enum type {
    VAR,
    FUNC
} type_t;

typedef struct {
    type_t type;
    bool decl;
    struct tBSTNode *content;
} Data;

typedef struct tBSTNode {
    char* Key;
    Data data;
    struct tBSTNode *LPtr;
    struct tBSTNode *RPtr;
} *tBSTNodePtr;

void BSTInit   (tBSTNodePtr *);
bool BSTSearch (tBSTNodePtr, char* , Data*);
void BSTInsert (tBSTNodePtr *, char*, type_t, bool);
void BSTDelete (tBSTNodePtr *, char*);
void BSTDispose(tBSTNodePtr *);


#endif //PROJIFJ_SYMTABLE_H
