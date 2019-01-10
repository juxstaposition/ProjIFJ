/*** 
 * Projekt IFJ2018
 * @brief       Header file for Precedence syntax analysis
 * @authors:    Romana Dzubarova,   xdzuba00
 *              Michal Vanka,       xvanka00
 *
 * Function definitons and precedence types
 */

#ifndef PRECEDENCE_TABLE_H
#define PRECEDENCE_TABLE_H


#include "lex_a.h"
#include "synt_a.h"
#include "symtable.h"
#include "errors.h"


typedef enum
{
    G, /// < greater
    E, /// = equal
    L, /// > less
    N  /// # error
}precedence_table_character;

typedef enum
{
    PLS,         /// +
    MIN,         /// -
    MUL,         /// *
    DIV,         /// /
    OpenBracket, /// (
    ClosBracket, /// )
    LES,         /// <
    LEQ,         /// <=
    MOR,         /// >
    MRE,         /// >=
    EQL,         /// ==
    NQE,         /// !=
    ID,          /// ID
    INT,         /// int number
    DOUBLE,      /// double number
    STRING,      /// string number
    ZNK,         /// symbolize < symbol
    DOLLAR,      /// $
    NON_TERM     /// E
}precedence_table_symbol;


typedef enum
{
    R_OPERAND,  /// E -> i
    R_BRACKET,  /// E -> (E)
    R_PLS,      /// E -> E + E
    R_MIN,      /// E -> E - E
    R_MUL,      /// E -> E * E
    R_DIV,      /// E -> E / E
    R_LES,      /// E -> E < E
    R_LEQ,      /// E -> E <= E
    R_MOR,      /// E -> E > E
    R_MRE,      /// E -> E >= E
    R_EQL,      /// E -> E == E
    R_NQE,      /// E -> E != E
    R_NOTR      ///not rule
}parser_rules;

typedef struct Stack_section
{
    precedence_table_symbol item;
    struct Stack_section* next_item;
} Stack_section_i;

typedef struct
{
    Stack_section_i* stack_top;
} Stack_symbol;


int Stack_symbol_push(precedence_table_symbol symbol, Stack_symbol* Stack);
void Stack_symbol_pop (Stack_symbol* Stack);
int Stack_symbol_pop_r (Stack_symbol* Stack);
Stack_section_i* Stack_item_on_top (Stack_symbol* Stack);
int Count_of_symbol_before_ZNK (Stack_symbol* Stack);
int Give_symbol_before_top (Stack_symbol* Stack, precedence_table_symbol symbol);
parser_rules reduce_expression_by_rules (Stack_symbol* Stack, int count);
int Precedence_SA (FILE* fl, tList* Ls);


#endif //PRECEDENCE_TABLE_PRECEDENCE_TABLE_H
