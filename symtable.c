/***
 * Projekt IFJ18
 * @name Symbol table for IFJ18 translator
 * @authors: Michal Vanka,      xvanka00
 */

#include "symtable.h"

/**
 * @brief Initializes bin tree root to NULL
 * @param RootPtr - Pointer to the beginning of the tree
 **/
void BSTInit (tBSTNodePtr *RootPtr) {
    *RootPtr = NULL;
}

/**
 * @brief Searches for specific var/func in bin tree and returns it's content
 * @param RootPtr - current root
 * @param name - name of the var/func
 * @param data - var for returning the content if found
 * @return - if found, returns true and content, if not, then returns false
 **/
bool BSTSearch (tBSTNodePtr RootPtr, char *name, Data *data)	{
    if (RootPtr == NULL)
        return false;
    else if (strcmp(RootPtr->Key,name) == 0) {
        *data = RootPtr->data;
        return true;
    }
    if (strcmp(RootPtr->Key,name) < 0)
        return BSTSearch(RootPtr->RPtr, name, data);
    else if (strcmp(RootPtr->Key,name) > 0)
        return BSTSearch(RootPtr->LPtr, name, data);
    return true;
}

/**
 * @brief Can be used to insert func into global bin tree, creating "content" subtree in data for vars,
 * or to insert vars into "content" subtree
 * @param RootPtr
 * @param name
 * @param type
 * @param decl
 **/
void BSTInsert (tBSTNodePtr* RootPtr, char *name, type_t type, bool decl)	{
    if (*RootPtr == NULL) {
        *RootPtr = malloc(sizeof(struct tBSTNode));
        (*RootPtr)->LPtr = NULL;
        (*RootPtr)->RPtr = NULL;
        (*RootPtr)->Key = name;
        (*RootPtr)->data.type = type;
        (*RootPtr)->data.decl = decl;
        (*RootPtr)->data.content = NULL;
    }
    else {
        if (strcmp((*RootPtr)->Key, name) == 0) {
            (*RootPtr)->data.type = type;
            (*RootPtr)->data.decl = decl;
        }
        else if (strcmp((*RootPtr)->Key, name) < 0)
            BSTInsert(&(*RootPtr)->RPtr, name, type, decl);
        else if (strcmp((*RootPtr)->Key, name) > 0)
            BSTInsert(&(*RootPtr)->LPtr, name, type, decl);
    }
}

/**
 * @brief Subfunction used by BSTDelete
 * @param PtrReplaced - Node to be replaced
 * @param RootPtr - Node used to replace
 **/
void ReplaceByRightmost (tBSTNodePtr PtrReplaced, tBSTNodePtr *RootPtr) {
    if ((*RootPtr) != NULL) {
        if ((*RootPtr)->RPtr == NULL) {
            PtrReplaced = (*RootPtr);
        }
        else
            ReplaceByRightmost(PtrReplaced, &(*RootPtr)->RPtr);
    }
}

/**
 * @brief Deletes single node in bin. tree, including "content" subtree in data
 * @param RootPtr - pointer to node to be deleted
 * @param name - name of the func/var
 **/
void BSTDelete (tBSTNodePtr *RootPtr, char *name) 	{
    if ((*RootPtr) != NULL) {
        tBSTNodePtr old = NULL;

        BSTDispose(&(*RootPtr)->data.content);

        if (strcmp((*RootPtr)->Key,name) == 0) {
            if ((*RootPtr)->RPtr != NULL && (*RootPtr)->LPtr != NULL) {
                tBSTNodePtr recursive_old = NULL;
                recursive_old = (*RootPtr);
                ReplaceByRightmost(recursive_old, &(*RootPtr)->LPtr);
                BSTDelete(&(*RootPtr)->LPtr, recursive_old->Key);
            }
            else if ((*RootPtr)->RPtr == NULL && (*RootPtr)->LPtr == NULL) {
                free(*RootPtr);
                *RootPtr=NULL;
            }
            else if ((*RootPtr)->RPtr == NULL && (*RootPtr)->LPtr != NULL) {
                old = (*RootPtr);
                (*RootPtr) = (*RootPtr)->LPtr;
                free(old);
                old = NULL;
            }
            else if ((*RootPtr)->RPtr != NULL && (*RootPtr)->LPtr == NULL) {
                old = (*RootPtr);
                (*RootPtr) = (*RootPtr)->RPtr;
                free(old);
                old = NULL;
            }
        }
        else if (strcmp((*RootPtr)->Key,name) < 0) {
            if ((*RootPtr)->RPtr!=NULL)
                BSTDelete(&(*RootPtr)->RPtr, name);
        }
        else if (strcmp((*RootPtr)->Key,name) > 0) {
            if ((*RootPtr)->LPtr!=NULL)
                BSTDelete(&(*RootPtr)->LPtr, name);
        }
    }
}
/**
 * @brief Disposes the whole tree
 **/
void BSTDispose (tBSTNodePtr *RootPtr) {
    if ((*RootPtr) != NULL) {
        if ((*RootPtr)->LPtr!=NULL)
            BSTDispose(&(*RootPtr)->RPtr);

        if ((*RootPtr)->RPtr != NULL)
            BSTDispose(&(*RootPtr)->LPtr);

        free(*RootPtr);
        *RootPtr = NULL;
    }
}