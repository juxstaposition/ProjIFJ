/***
 * Projekt IFJ18
 * @brief 	 Lexical Analysis in Translator of IFJ18 lang
 * @authors: Daniel Ocenas, xocena06
 *
 * Consists of Finite State Machine, which processes source file
 * and creates tokens. Tokens are created during next_token function
 * and are passed to symtable
 */

#include "lex_a.h"

/** 
 *	@int DA_state Set as global variable, because there is state which distunguis comment
 *				  at the beggining of the line and therefore, cannot be always set to initial
 *				  state when requesting token
 **/
int DA_state = DA_NEWLINE; 

/**
 * @Brief 	Keywords definition
 * 			
 * 			I decided to list keywords in const. char structure
 *			because symtable was not yed implemented in the time
 *			lexical analysis implementation
 **/
const char *key_words_list[KEY_WORDS_NUMBER] = {
	"def",
	"do",
	"else",
	"end",
	"if",
	"not",
	"nil",
	"then",
	"while"
};

/**
 * @brief 	Operators definition
 **/
const char *operators_list[OPERATORS_NUMBER] = {
	"+",
	"-",
	"*",
	"/",
	"(",
	")",
	"<",
	"<=",
	">",
	">=",
	"!=",
	","
};

/**
 * @brief 	Function realizes if IDF inserted in param str is keyword
 *
 * @param 	str String representation of IDF 
 * @return 	function returns 0 in case of no match str with keyword
 *			or returns position of keyword occurence
 **/
int key_word_search(char *str)
{
	for (int i = 0; i < KEY_WORDS_NUMBER; i++)
	{
		if (strcmp(str, key_words_list[i]) == 0)
		{
			return i+1;
		}
	}
	return 0;
}

/**
 * @brief 	Function converts ASCII value of hexadecimal char to integer
 *
 * @param 	hex Integer with ASCII value of hexadecimal char 
 * @return 	Function returns integer value of hexadecimal character in string format
 */
int hex_to_int(int hex)
{
	if ('a' <= hex && hex <= 'f') 
	{
		return hex - 'a';
	}
	else if ('A' <= hex && hex <= 'F')
	{
		return hex - 'A';
	}
	else
	{
		return hex - '0';
	}
}

/**
 * @brief 	Function purpose to save finite automat states which are stored the same way
 *
 * @param 	str String representation of operator 
 * @return 	Funtion return Token value of operator
 **/
int operator_search(char *str)
{
	for (int i = 0; i < OPERATORS_NUMBER; i++)
	{
		if (strcmp(str, operators_list[i]) == 0)
		{
			return i+16;			// operators in TokenType starts at index 16
		}
	}
	return 0;
}

/**
 * @Brief 	Function performs operations of finite automat and returns next token
 * 			depending on position in a source file
 *
 * @param 	fil Pointer to a source file
 * @return 	Function returns token with type according to the next lexem
 **/
Token* next_token(FILE *ptr)
{
	tQueue* queue = malloc(sizeof(tQueue));
	Token *token = malloc(sizeof(struct Token));


	// reserving space for Token
	if (token == NULL)
	{
		token->type = T_ERR_MALLOC;
		return token;
	}

	// reserving space for queue
	if ( queue_set(queue) == 1)
	{
		token->type = T_ERR_MALLOC;
		return token;
	}

	int token_acquired = FALSE,		/* terminates finite automat if token is acquired */
		EOF_reached = FALSE,		/* terminates loop over source file if EOF 
									   is reached and properly returns all tokens */
		string_err = FALSE, 		/* in case invalid string literal, helps to correctly quit string literal */
		block_comm = FALSE, 		/* stores state when inside block commentary */
		hex_nmr = 0, hex1, hex2;	/* accumulates occurences of hexadecimal number in string literal and its values */

	char c;							/* char transmitter */
	
	// while loop iterates over source file
	while (EOF_reached == FALSE)
	{
		c=fgetc(ptr);				// gets next char from a file

		// switch over integer DA_state and simulates finite automat
		switch (DA_state)
		{
			/* DA_START is defualt state which identifies incoming character
			   and determines next state of finite automat. */
			case DA_START :
			{
				if (c == ' ' || c == '\t' || c == '\r')			// White space escape sequence 
				{												// \r carriage return, escaoe sequence that may occur
					DA_state = DA_START;						// escapes it and returns to the same state
				}
				else if (c == '\n')								// if EOL found, token is returned
				{
					token->type = T_EOL;
					token_acquired = TRUE;

					DA_state = DA_NEWLINE; 	
				}
				else if (('a' <= c && c <= 'z') || (c == '_'))	// first valid char of identificator
				{	
					if ( (queueUp(queue,c)) == 1)
					{
						token->type = T_ERR_MALLOC;
						token_acquired = TRUE;
						// may be needed to set T/F state for malloc error
					}
					DA_state = DA_IDENTIFICATOR;				// next state is idf accumulation
				}
				else if ('0' <= c && c <= '9')					// first valid char of number
				{												
					if ( (queueUp(queue,c)) == 1)
					{
						token->type = T_ERR_MALLOC;
						token_acquired = TRUE;
					}
					DA_state = DA_NUMBER;						// next state is number accumulation
				}
				else if (c == '=')								// queal or assingment accumulation
				{
					if ( (queueUp(queue,c)) == 1)
					{
						token->type = T_ERR_MALLOC;
						token_acquired = TRUE;
					}
					DA_state = DA_EQUAL;
				}
				else if (c == '#')								// line commentary
				{
					DA_state = DA_LINECOMM;
				}
				else if (c == '"')								// string
				{
					DA_state = DA_STRING;
				}
				else if (c == '+' || c == '-' || c == '*' || c == '/' || c == '(' || 
						 c == ')' || c == '<' || c == '>' || c== '!'  || c == ',')					// *************** needs to be checked *******
				{
					if ( (queueUp(queue,c)) == 1)
					{
						token->type = T_ERR_MALLOC;
						token_acquired = TRUE;
					}
					DA_state = DA_OPERATOR;
				}
				else if (c == EOF)								// EOF reached
				{
					token->type = T_EOF;						// returns Token EOF
					EOF_reached = TRUE;							// loop is terminated
					token_acquired = TRUE;
				}
				else											// if symbol is not recognized, considered as undefined lexical behaviour
				{
					if (queue->arr != NULL)
						fprintf(stderr,"Lex error, invalid char: '%s'\n",queue->arr );
					token->type = T_ERR;
					token_acquired = TRUE;
					DA_state = DA_START;
				}

				break;
			}

			// escapes characters until it reaches end of line, then exits state of line commentary
			// cooperates with block commentary
			case DA_LINECOMM :
			{	
				if (c != '\n' && c != EOF)
				{
					DA_state = DA_LINECOMM; 
				}
				else if (c == EOF)
				{
					fprintf(stderr,"Lex error, unterminated commentary\n");
					token->type = T_ERR;
					token_acquired = TRUE;
					ungetc(c,ptr);
					DA_state = DA_START;
				}
				else
				{ 	
					if (block_comm == FALSE)
					{
						token->type = T_EOL;
						token_acquired = TRUE;
						ungetc(c,ptr);
					}

					DA_state = DA_NEWLINE; 
				}
				break;
			}

			//  purpose is to find block commentary begining and end which can be only at the beginning of the line
			case DA_NEWLINE :
			{ 	
				if (c == '=')
				{
					//test_qsize();
					if ( (queueUp(queue,c)) == 1)
					{
						token->type = T_ERR_MALLOC;
						token_acquired = TRUE;
					}
					DA_state = DA_GET_BLOCKCOMM;
				}
				else
				{		
					if (block_comm == FALSE)
					{		
						ungetc(c,ptr);
						DA_state = DA_START;
					}
					else
					{
						DA_state = DA_LINECOMM;
					}
				}
				break;

			}

			// state of block comm, detects beggining and end of block commentary
			// if found "=begin" at the beggining of a line, enters escaping sequence 
			// until "=end" is found at the beggining of a line
			// block comm is executed as line commentary with detection of its termination
			// via "=end" at the exact begining of a line
			case DA_GET_BLOCKCOMM :
			{
				// if whitespace char or EOF is found, stops accumulates chars from the beggining of a line
				if (c != ' ' && c != '\t' && c != '\n' && c != '\r' && c!= EOF)
				{
					if ( (queueUp(queue,c)) == 1)
					{
						token->type = T_ERR_MALLOC;
						token_acquired = TRUE;
					}
					DA_state = DA_GET_BLOCKCOMM;
				}
				else
				{
					// if not inside block commentary
					if (block_comm == FALSE)
					{	
						// detect proper beggin of a block comm
						if(strcmp(queue->arr,"=begin") == 0)
						{
							block_comm = TRUE;
							DA_state = DA_LINECOMM;
						}
						// if there is not corret beggining of a block comm, consideres as incorect lexem
						else
						{
							fprintf(stderr,"Lex error, invalid start of block commentary: '%s'\n",queue->arr );
							DA_state = DA_START;
						}
					}
					else	//block_comm == TRUE
					{
						// if end occurs, block comm is correctly terminated
						if(strcmp(queue->arr,"=end") == 0)
						{
							block_comm = FALSE;
						}
						// aways conntinues as line comment
						DA_state = DA_LINECOMM;
						
					}
					// needs to erase queue, thus free of the queue is perfomed and again new allocation
					clear_queue(queue);
					if (token_acquired != TRUE)
					{
						if ( (queue_set(queue)) == 1)
						{
							token->type = T_ERR_MALLOC;
							return token;
						}
					}

				}
				
				break;
			}

			// accumulates valid symbols of IDF
			case DA_IDENTIFICATOR :	
			{ 
				if (('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z') || ('0' <= c && c <= '9') || (c == '_') )
				{
					if ( (queueUp(queue,c)) == 1)
					{
						token->type = T_ERR_MALLOC;
						token_acquired = TRUE;
					}
					DA_state = DA_IDENTIFICATOR;						// while valid symbols come, they are accumulated
				}
				else if (c == '!' || c == '?'){
					if ( (queueUp(queue,c)) == 1)
					{
						token->type = T_ERR_MALLOC;
						token_acquired = TRUE;
					}
					token->type = T_IDF;
					token->data.str_cont = (char*) malloc(queue->b_index + 1);
					strncpy(token->data.str_cont, queue->arr,queue->b_index+1);
					token_acquired = TRUE;
					DA_state = DA_START;
				}
				else
				{
					int keyword_number = key_word_search(queue->arr);	// finds out whether buffer conntent is keyword

					if ( keyword_number != 0 )
					{
						token->type = (TokenType)keyword_number;		// if yes, it is automatically assigned to a token with correct token type
						token_acquired = TRUE;
					}
					else
					{
						token->type = T_IDF;
						token->data.str_cont = (char*) malloc(queue->b_index + 1);
						strncpy(token->data.str_cont, queue->arr,queue->b_index+1);
						token_acquired = TRUE;
					}

					ungetc(c,ptr); 					// because IDF accumulation was stopped, 
													// for proper function last symbol need to be used again
					DA_state = DA_START;			// returns back to initial state
				}
				break;
			}

			// Determines operator and creates token from it
			case DA_OPERATOR :			
			{
				if (c == '=')
				{
					if ( (queueUp(queue,c)) == 1)
					{
						token->type = T_ERR_MALLOC;
						token_acquired = TRUE;
					}		
				}
				else
				{
					ungetc(c,ptr);
				}

				int operator_number = operator_search(queue->arr);

				token->type = (TokenType)operator_number;				
				token_acquired = TRUE;

				DA_state = DA_START;
				break;
			}

			// sets either ASG or EQUAL token 
			case DA_EQUAL :			
			{	
				if (c == '=')
				{
					if ( (queueUp(queue,c)) == 1)
					{
						token->type = T_ERR_MALLOC;
						token_acquired = TRUE;
					}
					token->type = T_EQL;
					token_acquired = TRUE;
				}
				else
				{
					ungetc(c,ptr);
					token->type = T_ASG;
					token_acquired = TRUE;

				}

				DA_state = DA_START;
				break;
			}

			// reads string and stores it with token type string
			case DA_STRING :
			{
				if (c == '\\' )							// escape sequence symbol
				{
					DA_state = DA_STRINGESC;
				}
				else if (c != '"' ) 					// ends string accumulation
				{
					if ( (queueUp(queue,c)) == 1)
					{
						token->type = T_ERR_MALLOC;
						token_acquired = TRUE;
					}
					DA_state = DA_STRING; 
				}
				else 
				{ 		
					if (string_err == FALSE)
					{
						token->type = T_STR;
						token->data.str_cont = (char*) malloc(queue->b_index + 1);
						strncpy(token->data.str_cont, queue->arr,queue->b_index+1);
						token_acquired = TRUE;
					}
					else
					{
						token->type = T_ERR;
						token_acquired = TRUE;
					}

					DA_state = DA_START; 
				}

				break;
			}

			// option of putting symbol into string literal via escape sequence
			case DA_STRINGESC :
			{
				if (c == '"')
				{
					if ( (queueUp(queue,'\"')) == 1)
					{
						token->type = T_ERR_MALLOC;
						token_acquired = TRUE;
					}					
				}
				else if (c == 'n')
				{
					if ( (queueUp(queue,'\n')) == 1)
					{
						token->type = T_ERR_MALLOC;
						token_acquired = TRUE;
					}
					}
				else if (c == 't')
				{
					if ( (queueUp(queue,'\t')) == 1)
					{
						token->type = T_ERR_MALLOC;
						token_acquired = TRUE;
					}
				}
				else if (c == 's')
				{
					if ( (queueUp(queue,' ')) == 1)
					{
						token->type = T_ERR_MALLOC;
						token_acquired = TRUE;
					}
				}
				else if (c == '\\')
				{
					if ( (queueUp(queue,'\\')) == 1)
					{
						token->type = T_ERR_MALLOC;
						token_acquired = TRUE;
					}
				}
				else if (c == 'x')
				{
					DA_state = DA_HEXESC;
					break;
				}
				else
				{
					if ( (queueUp(queue,c)) == 1)
					{
						token->type = T_ERR_MALLOC;
						token_acquired = TRUE;
					}
				}

				DA_state = DA_STRING;

				break;
			}

			// 
			case DA_HEXESC :
			{
				// can repeat max 2 times
				// expect hexadecimal number -> symbols 0-9 && a - f, otherwise, symbol not taken
				if ( (('a' <= c && c <= 'f') || ('A' <= c && c <= 'F') || ('0' <= c && c <= '9')) && hex_nmr < 2 )
				{
					hex_nmr++;
					
					if (hex_nmr == 1)
					{
						hex1 = hex_to_int(c);
					}					
					else
					{
						hex2 = hex_to_int(c);
					}

					DA_state = DA_HEXESC;
					break;
				}

				ungetc(c,ptr);
				// in case of no occurence of hexadecimal character, consideres as lexical error
				if (hex_nmr == 0)
				{
					string_err = TRUE;
					fprintf(stderr,"Lex error, invalid hexadecimal format: \\x%c\n",c);
				}
				else if (hex_nmr == 1)				// one hexadecimal number can be stored right away after conversion
				{
					char hex = (char) hex1;
					if ( (queueUp(queue,hex)) == 1)
					{
						token->type = T_ERR_MALLOC;
						token_acquired = TRUE;
					}
				}
				else if (hex_nmr == 2)				// if 2 hex characters, the first one needs to e multiplied by 16
				{									// cause of conversion from dec to hex
					char hex = (char) (hex1 * 16 + hex2);
					if ( (queueUp(queue,hex)) == 1)
					{
						token->type = T_ERR_MALLOC;
						token_acquired = TRUE;
					}
				}
				hex_nmr = 0;
				DA_state = DA_STRING;
				break;
			}

			// reads integer and stores it with token type int
			case DA_NUMBER :
			{	
				if ('0' <= c && c <= '9')
				{
					if ( (queueUp(queue,c)) == 1)
					{
						token->type = T_ERR_MALLOC;
						token_acquired = TRUE;
					}
					DA_state = DA_NUMBER;
				}
				// if floating point occurs, changes automat state
				else if ( c == '.')
				{
					if ( (queueUp(queue,c)) == 1)
					{
						token->type = T_ERR_MALLOC;
						token_acquired = TRUE;
					}
					DA_state = DA_FLOAT;
				
				}
				// if exponential part, changes automat state
				else if ( c == 'e' || c == 'E')
				{
					if ( (queueUp(queue,c)) == 1)
					{
						token->type = T_ERR_MALLOC;
						token_acquired = TRUE;
					}
					DA_state = DA_EXPONENT;
				}
				// if no float or exponential expresion, and different symbols comes, storing INT 
				else
				{
					token->type = T_INT;
					token->data.int_value = atoi(queue->arr);			// save to use atoi, becuse automat isnt accumulation other symbols than numbers
					token_acquired = TRUE;

					ungetc(c,ptr);
					
					DA_state = DA_START; 
				}
				
				break;
			}

			// floating part of number with floating point, case with exponent may occur
			case DA_FLOAT:
			{
				if ('0' <= c && c <= '9')
				{
					if ( (queueUp(queue,c)) == 1)
					{
						token->type = T_ERR_MALLOC;
						token_acquired = TRUE;
					}
					DA_state = DA_FLOAT;
				}
				else if ( c == 'e' || c == 'E')
				{
					if ( (queueUp(queue,c)) == 1)
					{
						token->type = T_ERR_MALLOC;
						token_acquired = TRUE;
					}
					DA_state = DA_EXPONENT;
				}
				else
				{
					// needs to check char before first free space in queue -> the last char in queue
					// if it is "." thus no number was yet added, it is considered as lexical error
					if (queue->arr[queue->b_index-1] != '.')		
					{
						token->type = T_DOUBLE;
						token->data.double_value = atof(queue->arr);					
						token_acquired = TRUE;
					}
					else
					{
						token->type = T_ERR;
						fprintf(stderr,"Lex error, invalid FLOAT lexem: '%s'\n",queue->arr );					
						token_acquired = TRUE;
					}
					ungetc(c,ptr);
					
					DA_state = DA_START; 
				}
				break;
			}

			// exponential part of number
			case DA_EXPONENT:
			{
				// optional symbol '+' or '-' only after start of exponential sequence
				if (queue->arr[queue->b_index-1] == 'e' || queue->arr[queue->b_index-1] == 'E')		
				{
					if ( c == '+' || c == '-')
					{
						if ( (queueUp(queue,c)) == 1)
						{
							token->type = T_ERR_MALLOC;
							token_acquired = TRUE;
						}
						DA_state = DA_EXPONENT;
						break;
					}
				}

				if ('0' <= c && c <= '9')
				{
					if ( (queueUp(queue,c)) == 1)
					{
						token->type = T_ERR_MALLOC;
						token_acquired = TRUE;
					}
					DA_state = DA_EXPONENT;
				}
				else
				{
					// needs to check char before first free space in queue -> the last char in queue
					// if it is "." thus no number was yet added, it is considered as lexical error
					if (queue->arr[queue->b_index-1] != 'e' || queue->arr[queue->b_index-1] != 'E')		
					{
						token->type = T_DOUBLE;
						token->data.double_value = atof(queue->arr);		// save to use atof, accumulation only correct symbols			
						token_acquired = TRUE;
					}
					else
					{
						token->type = T_ERR;
						fprintf(stderr,"Lex error, invalid FLOAT lexem: '%s'\n",queue->arr );					
						token_acquired = TRUE;
					}
					ungetc(c,ptr);
					
					DA_state = DA_START; 

				}
				break;			
			}

			default:
			{
				DA_state = DA_START;
				break;
			}
		} // end switch

		if ( token_acquired == TRUE )
		{
			clear_queue(queue);
			free(queue);
			return token;
		}

	} // end while
	
	token->type = T_ERR;
	return token;

}