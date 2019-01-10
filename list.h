/***
 * Projekt IFJ2018
 * @Brief	Header file for list
 * @Author: Daniel Ocenas, xocena06
 *
 * Defines structure of list, which is used for token storage during 
 * syntactit analysis. Also defines function prototypes.
 */

#include "lex_a.h"

#ifndef LIST_H
#define LIST_H
  
typedef struct tElem {                 	/* element of DList */ 
        Token dataList;                 /* Data of element in the list */
        struct tElem *lptr;          	/* Pointer to the previous element */
        struct tElem *rptr;        		/* Pointer to the next item */
} *tElemPtr;

typedef struct {                        /* Structure type DList */
    tElemPtr First;                     /* Pointer to the first element of DList */
    tElemPtr Act;                       /* Pointer to the active element of DList */
    tElemPtr Last;                      /* Pointer to the last element of DList */
	int elem_count;					/* pocet polozek seznamu */
} tList;

/* Function prototypes */
void InitList (tList *);
void DisposeList (tList *);
void InsertFirst (tList *, Token);
void First (tList *);
void Last (tList *);
void DeleteFirst (tList *);				
void PostInsert (tList *, Token);
void Succ (tList *);
void Pred (tList *);

#endif // LIST_H