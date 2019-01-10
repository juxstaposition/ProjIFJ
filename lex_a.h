/***
 * Projekt IFJ18
 * @brief	Header file for lexical analysis in Translator of IFJ18 lang
 * @author: Daniel Ocenas, xocena06
 *
 * Defines states of finite automat,declaration of Token struct 
 * and its types and function prototypes.
 */


#ifndef _LEX_
#define _LEX_

#include "queue.h"
#include <string.h>

/**
 * Defining states of finite state machine (FSM)
 **/
#define DA_START 			0		// initial state, means ready to acquire new tokens
#define DA_NEWLINE			1		// start of a line
#define DA_LINECOMM			2	 	// line commentary escape
#define DA_GET_BLOCKCOMM	3		// block commentary evaluation
#define DA_IDENTIFICATOR	4	    // identificator
#define DA_STRING			5		// string sequence
#define DA_OPERATOR			6		// operator
#define DA_COMPARISON		7	  	// comparison operator
#define DA_NUMBER			8		// number 
#define DA_EQUAL			9		// equal
#define DA_EXPONENT			10		// exponential part of floating point number
#define DA_FLOAT			11		// floating part of a number
#define DA_STRINGESC		12		// string escape sequence
#define DA_HEXESC			13		// hexa char escape sequence

/**
 * Macros
 **/
#define KEY_WORDS_NUMBER	9		// ammount of key words for array declaration
#define TOKENS_NUMBER		33		// ammount of token types
#define OPERATORS_NUMBER	12		// ammount of operators
#define TRUE 				1		// macro for TRUE
#define FALSE				0		// macro for FALSE

typedef enum ENUMTokenType
{
	T_ERR,			// error
	// keywords
	T_DEF,			// kw def	
	T_DO,			// kw do
	T_ELSE,			// kw else
	T_END,			// kw end
	T_IF,			// kw if
	T_NOT,			// kw not
	T_NIL,			// kw nil
	T_THEN,			// kw then
	T_WHILE,		// kw while
	T_EOL,			// EOL
	T_IDF,			// identificator
	T_STR,			// string
	T_INT,			// integer
	T_DOUBLE,		// double
	T_EOF,			// EOF
	// operators start idx 16
	T_PLS,			// +
	T_MIN,			// -
	T_MUL,			// *
	T_DIV,			// /
	T_OpenBracket,	// (
	T_ClosBracket,	// )
	T_LES,			// <
	T_LEQ,			// <=
	T_MOR,			// >
	T_MRE,			// >=
	T_NQE,			// !=
	T_CLM,			// ,
	T_ASG, 			// = 
	T_EQL,			// ==
	T_GRD,			// #
	T_QoutMark,		// "
	T_ERR_MALLOC,	// malloc error for proper return code

} TokenType;

typedef struct Token
{
	TokenType type;				// Stores type of token
	union						// Need to use max 1 of 3 different datatypes, 
	{							// int double or string
		int int_value;			// thus union is perfect struture 
		double double_value;
		char* str_cont;
	} data; 

} Token;

/**
 * Function prototypes
 **/
int hex_to_int(int hex);
void lex_err(char *type, char *lexem);
int key_word_search(char *str);
int operator_search(char *str);
Token* next_token(FILE *ptr);

#endif //_LEX_