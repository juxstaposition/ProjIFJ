/***
 * Projekt IFJ18
 * @name    Error Types
 * @authors Scavnicka Sarka (xscavn01)
 *
 * Defines proper return codes according to the task description
 */

#ifndef ERRORS_H_INCLUDED
#define ERRORS_H_INCLUDED

typedef enum Error_Codes
{
    // If there is no error in the translation, the return value returns 0
    NO_ERROR                    = 0,
    // Error in a lexical analysis program (bad structure of the current lex)
    LEXICAL_ERROR               = 1,
    // Program syntax error (syntax error)
    SYNTAX_ERROR                = 2,
    // Semantic bug in program - undefined function/variable, etc...
    SEMANTIC_DEFINITION_ERROR   = 3,
    // Semantic/run eror of type compatibility in arithmetic, chain, and relational expressions
    SEMANTIC_DATATYPE_ERROR     = 4,
    // Semantic error in the program - bad number of parameters for function call
    SEMANTIC_PARAMETERS_ERROR   = 5,
    // Other semantic errors
    SEMANTIC_OTHER_ERROR        = 6,
    // Division by zero error
    ZERO_DIV_ERROR              = 9,
    // The internal error of the translator, affected by available programs,
    // such as an allocation error
    INTERNAL_ERROR              = 99,
} Error_Codes; //Program errors


#endif // ERRORS_H_INCLUDED
