/***
 * Projekt IFJ2018
 * @name Syntactit Analysis (parser)
 * @authors:    Daniel Miloslav Očenáš, xocena06
 *              Michal Vanka,           xvanka00
 *
 * Syntactit analysis in our project for Language IFJ18
 * Parser scans a source file. From lexical analysis are acquired tokens
 * which are processed according to the rules defined in LL grammar.
 * Grammar is included in the documentation.
 *
 * Parser works on principle of comparing tokens from the source file
 * with expected sequence of tokens defined in grammar. 
 * If a token match with grammar rule, next token is called from 
 * the source file and are inserted into doublelist. While scanning rules,
 * tokens may not match and thanks to list we can properly work with them
 * thanks to active element in the list of tokens.
 */

#include "synt_a.h"

int err_code = NO_ERROR;
tBSTNodePtr *global_symtable;

// for tokenprint
const char *TOKENTYPE[TOKENS_NUMBER]=
{
    "T_ERR",            // error
    "T_def",            // kw def   
    "T_do",             // kw do
    "T_else",           // kw else
    "T_end",            // kw end
    "T_if",             // kw if
    "T_not",            // kw not
    "T_nil",            // kw nil
    "T_then",           // kw then
    "T_while",          // kw while
    "T_EOL",            // EOL
    "T_IDF",            // identificator
    "T_STR",            // string
    "T_INT",            // integer
    "T_DOUBLE",         // double
    "T_EOF",            // EOF
    "T_PLS",            // +
    "T_MIN",            // -
    "T_MUL",            // *
    "T_DIV",            // /
    "T_OpenBracket",    // (
    "T_ClosBracket",    // )
    "T_LES",            // <
    "T_LEQ",            // <=
    "T_MOR",            // >
    "T_MRE",            // >=
    "T_NQE",            // !=
    "T_CLM",            // ,
    "T_ASG",            // = 
    "T_EQL",            // ==
    "T_GRD",            // #
    "T_QoutMark",       // "
    "T_ERR_MALLOC",
};

const char *functions[]=
{
    "print",
    "length",
    "substr",
    "inputs",
    "inputi",
    "inputf",
    "chr",
    "ord",
    "begin",
};

/**
 *
 *
 **/
void list_to_symtable(tList *L,int res){
    for (int i = 0; i<res; i++){
        if (L->First->dataList.type == T_IDF)
        {
            printf("idf\n");
        }
        DeleteFirst(L);
    }
}


/**
 * @brieg Function used to call and execute syntactic analysis
 * @param filename Pointer to a sourcefile, which will be translated and compiled.
 * @retun function returns int value. 0 in case of proper exection, err_code otherwise
 **/
int synt_a(char *filename)
{
    // file initiation
    FILE *ptr;
    if ((ptr = fopen(filename,"r")) == NULL) {
        fprintf(stderr,"Could not open file:%s\n", filename);    // handles error at opening file
        return INTERNAL_ERROR;
    }

    // list initiation
    tList *TokenList = malloc(sizeof(tList));
    if (TokenList == NULL) {
        return INTERNAL_ERROR;        
    }    
    InitList(TokenList);

    // symtable initiation.
    global_symtable = malloc(sizeof(struct tBSTNode));
    if (global_symtable == NULL) {
        return INTERNAL_ERROR;        
    }
    BSTInit(global_symtable);

    Token *token = next_token(ptr);
    InsertFirst(TokenList,*(token));
    delete_token(token);
    First(TokenList);
    if (TokenList->First->dataList.type == T_ERR)
    {
        return LEXICAL_ERROR;
    }
    else if (TokenList->First->dataList.type == T_ERR_MALLOC)
    {
        return T_ERR_MALLOC;
    }

    int rule_A = 0;

    while (rule_A != 2)
    {
        rule_A = a_rule(ptr,TokenList);
        if (rule_A == 1){
            printf("err_code%d\n",err_code );
        	if(err_code == NO_ERROR){
	      	 	err_code = SYNTAX_ERROR;
/* edit*/		fprintf(stderr, "syntactic error");
	        }
/* print*/	print_list(TokenList);
  	    	DisposeList(TokenList);        	
        	break;
        }
    }

    BSTDispose(global_symtable);
    free(global_symtable);
    DisposeList(TokenList);
    free(TokenList);
    fclose(ptr);
/* print*/    printf("returning: %d.\n",err_code );
    return err_code;

}

/**
 * @brief Function frees token, which comes from lex analysis.
 * @param token is pointer to a token, to be freed
 **/
void delete_token(Token *token){
    if ( token->type == T_IDF || token->type == T_STR)
    {
       //free(token->data.str_cont);                             
    }
    
    free(token);
}

bool check_term(tList *L){
    if (L->Act->dataList.type == T_STR || L->Act->dataList.type == T_INT ||
        L->Act->dataList.type == T_IDF || L->Act->dataList.type == T_DOUBLE ||
        L->Act->dataList.type == T_NIL ){
        return TRUE;
    }
    else{
        return FALSE;
    }
}

/**
 * @brief Intelligent function which calls tokens
 * @param fil Pointer to a file, our source file
 * @param L   Pointer to a List in which is token added
 *
 * Functions sets active token according to elements in list
 * If there is an item in list, we can just move activity to the next item.
 * If no token is followed, lex analysis is called to retrieve a token.
 **/
void get_token(FILE *fil, tList *L){
    Token *ret = NULL;
    if (L->Act != NULL && L->Act->dataList.type == T_EOF)
    {
        printf(".....................................EOF\n");
        PrintAct(L);
        printf(".....................................EOF\n");
        ret = &(L->Act->dataList);
    }
    else if (L->Act->rptr == NULL )
    {
        Token *token;
        token = next_token(fil);
        PostInsert(L,*(token));
        delete_token(token);
        Succ(L);
        ret = &(L->Act->dataList);
    }
    else if (L->Act->rptr != NULL) 
    {
        Succ(L);
        ret = &(L->Act->dataList);
    }
    
    if (ret->type == T_ERR)
    {
        if (err_code == NO_ERROR){
            err_code = LEXICAL_ERROR;
        }
    }
    else if (ret->type == T_ERR_MALLOC){
        if (err_code == NO_ERROR){
            err_code = LEXICAL_ERROR;
        }
    }

    // debug
    printf("\t\tToken get: %s ",TOKENTYPE[ret->type] );
    if (ret->type == T_IDF || ret->type == T_STR)
    {
        printf("'%s'\n",ret->data.str_cont );
    }
    else
        printf("\n");
    // end debug
}

/**
 * @brief Function PrintAct used while debbuging functionality of parser
 **/
void PrintAct(tList *L){
    if (L->Act != NULL)
    {
        printf("Active Token: %s.\n", TOKENTYPE[L->Act->dataList.type]);
    }
}

/**
 * @brief Function print_list used while debbuging functionality of parser
 **/
void print_list(tList *L){
    if (L->First == NULL)
    {
        return;
    }

    tElemPtr item = L->First;
    printf("*********************\n");
    do
    {
        printf("Token: %s ",TOKENTYPE[item->dataList.type] );
        if (item->dataList.type == T_IDF || item->dataList.type == T_STR) {
            printf("%s\n",item->dataList.data.str_cont );
        }
        else {
            printf("\n");
        }
        item = item->rptr; 

    } while (item != NULL);
    
    printf("_____________________\n"); 
}

void move_back(tList *L, int n){
    for (int i = 0; i<n;i++)
        Pred(L);
}

/**
 * @brief Function a_rule is rule A / <prog> defined in grammar
 * @param fil Pointer to a file
 * @params L  Pointer to a list
 * @return    returns 0 if synt error, 1 if found rule 1, 2 if found eof
 *
 * Rule A mainly used as a calling function or for EndOfFile determining
 * and parser sucession
 **/
int a_rule(FILE *fil, tList *L){
    printf("a_rule start\n");
    int alt = START_CASE,     /* rule option state */
        alt_bool = FALSE,       /* use for iterate over rules */
        rule = 0;

    while (alt_bool == FALSE) {
        switch (alt) {
            case START_CASE :    // 1. A -> B A
            {
                int match = START_CASE, match_bool = FALSE;
                printf("1. A -> B A'\n" );
                while (match_bool == FALSE) {
                    switch (match) {
                        case START_CASE: // B
                        {
                            int res = b_rule(fil,L);
                            printf("*********************result: %d. err_code: %d.\n",res,err_code );
                            if ( res != MISSMATCH) {
                            	if(err_code != NO_ERROR)
                            	{
                            		return SYNTAX_ERROR;
                            	}
                                match++;
                                printf("RULE DONE\n");
                                print_list(L);
                                list_to_symtable(L,res);
                                printf("LISTSIZE = %d\n",L->elem_count );
                                print_list(L);
                                PrintAct(L);
                            }
                            else {
                                match_bool = TRUE;
                                alt = 1;
                            }
                            break;
                        }
                        case CASE_1: // A
                        {
                            int a_result = a_rule(fil,L);
                            if ( a_result == 1 ) {
                                rule = 1;
                                alt_bool = TRUE;
                            }
                            else if (a_result == 2 ){
                                rule = 2;
                                alt_bool = TRUE;
                            }
                            else {
                                alt = 1;
                            }

                            match_bool = TRUE;
                            
                            break;
                        }
                    } // end switch match

                } // end while 
                break;
            }
            case CASE_1 : // A -> EOF
            {
/* print */     printf("A -> EOF\n" );
/* print */    	PrintAct(L);
                if (L->Act->dataList.type == T_EOF) {
                    rule = 2;
                }
                else{
                    rule = 1;
                }
                
                alt_bool = TRUE;

                break;
            }
        
        } // end switch rule options 
    } // end while
    printf("a_rule end_______%d\n",rule);
    return rule;
}

/**
 * @brief Function b_rule is rule B / <stat> defined in grammar
 * @param fil Pointer to a file
 * @params L  Pointer to a list
 * @return    returns -1 if rule any rule does not match -> parser err
                      or ammount of used tokens in a rule
 **/
int b_rule(FILE *fil, tList *L){
    printf("b_rule start\n");
    int alt_bool = TRUE,       /* use for iterate over rules */
        alt = START_CASE,                /* rule option state */
        rule = 0;

    while (alt_bool) {
        switch (alt) {
            case START_CASE: // E -> EOL
            {
                printf("E -> EOL\n");
                PrintAct(L);
                if (L->Act->dataList.type == T_EOL){
                    get_token(fil,L);
                    alt_bool=FALSE;
                    rule++;
                                printf("rule:%d\n",rule);
                }
                else{
                    alt++;
                    rule = 0;
                }

                break;
            }

            case CASE_1 :    // B -> def id ( G EOL D
            {
/* print */     printf("B -> def id ( G EOL D\n");
                int match = START_CASE, match_bool = TRUE;
                while (match_bool) {
                    switch (match) {
                        case START_CASE: // def
                        {
/* print */                	PrintAct(L); 
                            if (L->Act->dataList.type == T_DEF) {
                                PrintAct(L);
                                get_token(fil,L);
                                PrintAct(L);
                                rule++;
                                match++;
                            }
                            else {
                                match = MISSMATCH;
                            }
                            break;
                        }
                        case CASE_1: // idf
                        {
/* print */                	PrintAct(L); 
                            if (L->Act->dataList.type == T_IDF) {
                                get_token(fil,L);
                                rule++;
                                match++;
                            }
                            else 
                                match = MISSMATCH;
                            break;
                        }
                        case CASE_2: // (
                        {
/* print */                	PrintAct(L); 
                            if (L->Act->dataList.type == T_OpenBracket) {
                                get_token(fil,L);
                                rule++;
                                match++;
                            }
                            else 
                                match = MISSMATCH;
                            break;
                        }
                        case CASE_3: // G
                        {
                        	int items = g_rule(fil,L);
                            if ( items != MISSMATCH) {
                            	rule+= items;
                                match++;
                            }
                            else 
                                match = MISSMATCH;
                            break;
                        }                     
                        case CASE_4: // EOL
                        {
/* print */                	PrintAct(L);   
                            if (L->Act->dataList.type == T_EOL) {
                                get_token(fil,L);
                                rule++;
                                match++;
                            }
                            else
                            	match = MISSMATCH;
                            
                            break;
                        }
                        case CASE_5: // D
                        {   
                        	int items = d_rule(fil,L);                             
                            if ( items != MISSMATCH ) {
                                rule += items;
                                match_bool = FALSE;
                                alt_bool = FALSE;
                                printf("rule:%d\n",rule);
                            }
                            else 
                            	match = MISSMATCH;
                            
                            break;
                        }

                        default :
                        {
                        	move_back(L,rule);
                         	rule = 0;
                            match_bool = FALSE;
                            alt++;
                            break;
                        }

                    } // end switch match
                } // end while 
                break;
            }

            case CASE_2: // B -> if I then EOL C
            {
                printf("B -> if I then EOL C\n");
                int match = START_CASE, match_bool = TRUE;
                while (match_bool) {
                    switch (match) {
                        case START_CASE: // if
                        {
/*print */                      	PrintAct(L);
                            if ( L->Act->dataList.type == T_IF) {
                            	get_token(fil,L);
                            	rule++;
                                match++;
                            }
                            else {
                                match = MISSMATCH;
                            }
                            break;
                        }
                        case CASE_1: // I
                        {
                        	int items = i_rule(fil,L);
                            if ( items != MISSMATCH) {
                            	rule += items;
                                match++;
                            }
                            else {
                                match = MISSMATCH;
                            }
                            break;
                        }
                        case CASE_2: // then
                        {
/* print */                	PrintAct(L); 
                            if ( L->Act->dataList.type == T_THEN) {
                            	get_token(fil,L);
                            	rule++;
                                match++;
                            }
                            else {
                                match = MISSMATCH;
                            }
                            break;
                        }
                        case CASE_3: // EOL
                        {
/* print */                	PrintAct(L); 
                            if ( L->Act->dataList.type == T_EOL) {
                            	get_token(fil,L);
                            	rule++;
                                match++;
                            }
                            else {
                                match = MISSMATCH;
                            }
                            break;
                        }
                        case CASE_4: // C
                        {
                        	int items = (c_rule(fil,L));
                            if ( items != MISSMATCH ) {
                                rule+=items;
                                match_bool = FALSE;
                                alt_bool = FALSE;
                                printf("rule:%d\n",rule);
                            }
                            else {
                                match = MISSMATCH;
                            }
                            break;
                        }                        
                        default :
                        {
                        	move_back(L,rule);
                        	rule = 0;
                            match_bool = FALSE;
                            alt++;
                            break;
                        }

                    } // end switch match
                }   // end while 2
                break;
            } // end case 2
            case CASE_3: // B -> while I do EOL D
            {
                printf("B -> while I do EOL C\n");
                printf("while items %d\n",rule );
                int match = START_CASE, match_bool = TRUE;
                while (match_bool) {
                    switch (match) {
                        case START_CASE:                          // while
                        {
/* print */                	PrintAct(L); 
                            if ( L->Act->dataList.type == T_WHILE) {
                            	get_token(fil,L);
                            	rule++;
                                match++;
                            }
                            else {
                                match = MISSMATCH;
                            }
                            break;
                        }
                        case CASE_1: // I
                        {
                        	int items = i_rule(fil,L);
                            if (items != MISSMATCH ) {
                            	rule += items;
                                match++;
                            }
                            else {
                                match = MISSMATCH;
                            }
                            break;
                        }
                        case CASE_2: // do
                        {
/* print */                	PrintAct(L); 
                        	if ( L->Act->dataList.type == T_DO) {
                            	get_token(fil,L);
                            	rule ++;
                                match++;
                            }
                            else {
                                match = MISSMATCH;
                            }
                            break;
                        }
                        case CASE_3: // EOL
                        {
/* print */                	PrintAct(L); 
                            if ( L->Act->dataList.type == T_EOL) {
                            	get_token(fil,L);
                            	rule++;
                                match++;
                            }
                            else {
                                match = MISSMATCH;
                            }
                            break;
                        }
                        case CASE_4: // d
                        {
                        	int items = d_rule(fil,L);
                            printf("items %d, rule %d\n",items, rule );
                            if ( items != MISSMATCH ) {
                                rule += items;
                                match_bool = FALSE;
                                alt_bool = FALSE;
                                printf("rule:%d\n",rule);
                            }
                            else {
                                match = MISSMATCH;
                            }
                            break;
                        }                        
                        default :
                        {
                            printf("default while\n");
                        	move_back(L,rule);
                        	rule=0;
                            match_bool = FALSE;
                            alt++;
                            break;
                        }

                    } // end switch match
                }   // end while 3
                break;
            } // end case 3
            case CASE_4: //  B -> print F
            {
            	printf("B -> print F\n");
                int match = START_CASE, match_bool = TRUE;
                while (match_bool) {
                    switch (match) 
                    {
                        case START_CASE: // print
                        {
/* print */               	PrintAct(L); 
							if (L->Act->dataList.type == T_IDF){
                           		if (strcmp(L->Act->dataList.data.str_cont, "print") == 0){
	                            	get_token(fil,L);
	                            	rule++;
	                                match++;
                            	}
                            	else
                              	  match = MISSMATCH;	
                         	}
                            else
                                match = MISSMATCH;
                            break;
                        }
                		case CASE_1: // F
                		{
                			int items = f_rule(fil,L);
                            if ( items != MISSMATCH ) {
                            	match_bool = FALSE;
                            	alt_bool = FALSE;
                            	rule+=items;
                                printf("rule:%d\n",rule);
                            }
                            else {
                                match = MISSMATCH;
                            }
                            break;
                		}
                		default :
                		{
                			move_back(L,rule);
                			rule = 0;
                			alt++;
                			match_bool = FALSE;
                			break;
                		}
                	} // end switch
                }// end while 
            	break;
            }

            case CASE_5: // B -> id = H
            {
                printf("B -> id = H\n");
                int match = START_CASE, match_bool = TRUE;
                while (match_bool) {
                    switch (match) {
                        case START_CASE:                          // idf
                        {
/* print */                 PrintAct(L); 
                            if ( L->Act->dataList.type == T_IDF) {
                                get_token(fil,L);
                                rule++;
                                match++;
                            }
                            else 
                                match = MISSMATCH;
                            
                            break;
                        }
                        case CASE_1: // = 
                        {
/* print */                 PrintAct(L); 
                            if (L->Act->dataList.type == T_ASG) {
                                get_token(fil,L);
                                rule++;
                                match++;
                            }
                            else 
                                match = MISSMATCH;
                            
                            break;
                        }

                        case CASE_2: // H
                        {
                            int items = h_rule(fil,L);
                            if ( items != MISSMATCH ) {
                                match_bool = FALSE;
                                rule += items;
                                alt_bool = FALSE;
                                printf("rule:%d\n",rule);
                            }
                            else 
                                match = MISSMATCH;
                            break;
                        }

                        default :
                        {
                            move_back(L,rule);
                            rule=0;
                            match_bool = FALSE;
                            alt++;
                            break;
                        }

                    } // end switch match
                }   // end while 5

                break;
            } // end case 5

            case CASE_6:
            {
                printf("B -> H\n");
                int items = h_rule(fil,L);
                if ( items != MISSMATCH ) {
                    rule += items;
                }
                else{
                    rule = MISSMATCH;
                }
                alt_bool = FALSE;
                break;
            }

        } // end switch rule options 
    } // end while
    printf("b_rule end_______%d\n",rule);
    return rule;
}

/**
 * @brief Function c_rule is rule C / <then> defined in grammar
 * @param fil Pointer to a file
 * @params L  Pointer to a list
 * @return    returns -1 if rule rule does not carries out
 *                    or ammount of used tokens in a rule
 *
 * Checks then branch in if statement and searches for else Token
 * to properly execute the rule.
 **/
int c_rule(FILE *fil, tList *L)
{
    printf("c_rule start\n");
    int alt = START_CASE,                /* rule option state */
        alt_bool = TRUE,                 /* use for iterate over rules */
        rule = 0;

    while (alt_bool ) {
        switch (alt) {
            case START_CASE :    // C -> B C 
            {
                printf("C -> B C\n");
                int match = START_CASE, match_bool = TRUE;
                while (match_bool ) {
                    switch (match) {
                        case START_CASE: // B
                        {
                        	int items = b_rule( fil,L);
                            if ( items != MISSMATCH ) {
                            	rule+=items;
                                match++;
                            }
                            else {
                                match = MISSMATCH;
                            }
                            break;
                        }
                        case CASE_1: // C
                        {              
                        	int items = c_rule(fil,L);
                            if ( items != MISSMATCH) {
                                rule += items;
                                match_bool = FALSE;
                                alt_bool = FALSE;
                                printf("rule:%d\n",rule);
                            }
                            else {
                                match = MISSMATCH;
                            }
                            break;
                        }
                        default :
                        {
                        	move_back(L,rule);
                        	rule = 0;
                            match_bool = FALSE;
                            alt++;
                            break;
                        }

                    } // end switch match

                } // end while 

                break;

            }

            case CASE_1 : // C -> else EOL
            {
                printf("C -> else EOL C2\n");
                int match = START_CASE, match_bool = TRUE;
                while (match_bool ) {
                    switch (match) {
                        case START_CASE:
                        {
/* print */                	PrintAct(L); // else
                            if (L->Act->dataList.type == T_ELSE) {
                            	get_token(fil,L);
                            	rule++;
                                match++;
                            }
                            else {
                                match = MISSMATCH;
                            }
                            break;
                        }

                        case CASE_1:                                // EOL
                        {
/* print */                	PrintAct(L); 
                            if (L->Act->dataList.type == T_EOL) {
                                get_token(fil,L);
                                rule++;
                                match++;
                            }
                            else {
                                match = MISSMATCH;
                            }
                            break;
                        }
                        case CASE_2:                                // C2
                        {
                            int item = c2_rule(fil,L);
                            if (item != MISSMATCH){
                                rule += item;
                                match_bool = FALSE;
                                alt_bool = FALSE;
                                printf("rule:%d\n",rule);
                            }
                            else{
                                match = MISSMATCH;
                            }
                            break;
                        }

                        default :
                        {
                            move_back(L,rule);
                            rule = -1;
                            match_bool = FALSE;
                            alt_bool = FALSE;
                            break;
                        }

                    } // end switch match
                } // end while 1

                break;
            } // end case 1

            default:
            {
                alt_bool = FALSE;
            }
 
        } // end switch rule options 
    } // end while

    printf("C_rule end_______%d\n",rule);
    return rule;
}

/**
 * @brief Function c2_rule is rule C2 / <else> defined in grammar
 * @param fil Pointer to a file
 * @params L  Pointer to a list
 * @return    returns -1 if rule rule does not carries out
 *                    or ammount of used tokens in a rule
 *
 * In compariso to c_rule check syntax of else branch
 **/
int c2_rule(FILE *fil, tList *L){
    printf("C2_rule start\n");
    int alt = START_CASE,                /* rule option state */
        alt_bool = TRUE,       /* use for iterate over rules */
        rule = 0;

    while (alt_bool ) {
        switch (alt) {
            case START_CASE :    // C2 -> B C2 
            {
                printf("C2 -> B C2\n");
                int match = START_CASE, match_bool = TRUE;
                while (match_bool ) {
                    switch (match) {
                        case START_CASE: // B
                        {
                            int items = b_rule( fil,L);
                            if ( items != MISSMATCH ) {
                                rule+=items;
                                match++;
                                printf("rule:%d\n",rule);
                            }
                            else {
                                match = MISSMATCH;
                            }
                            break;
                        }
                        case CASE_1: // C2
                        {              
                            int items = c2_rule(fil,L);
                            if ( items != MISSMATCH) {
                                rule += items;
                                match_bool = FALSE;
                                alt_bool = FALSE;
                                printf("rule:%d\n",rule);
                            }
                            else {
                                match = MISSMATCH;
                            }
                            break;
                        }
                        default :
                        {
                            move_back(L,rule);
                            rule = 0;
                            match_bool = FALSE;
                            alt++;
                            break;
                        }

                    } // end switch match

                } // end while 

                break;

            }

            case CASE_1 : // C2 -> end EOL
            {
                printf("C2 -> end EOL \n");
                int match = START_CASE, match_bool = TRUE;
                while (match_bool ) {
                    switch (match) {
                        case START_CASE:
                        {
/* print */                 PrintAct(L);                            // end
                            if (L->Act->dataList.type == T_END) {
                                get_token(fil,L);
                                rule++;
                                match++;
                            }
                            else {
                                match = MISSMATCH;
                            }
                            break;
                        }
                        case 1:                                     // EOL
                        {
/* print */                 PrintAct(L); 
                            if (L->Act->dataList.type == T_EOL) {
                                get_token(fil,L);
                                rule++;
                                match_bool = FALSE;
                                alt_bool = FALSE;
                                printf("rule:%d\n",rule);
                            }
                            else {
                                match = MISSMATCH;
                            }
                            break;
                        }
                        default :
                        {
                            move_back(L,rule);
                            rule = -1;
                            match_bool = FALSE;
                            alt_bool = FALSE;
                            break;
                        }

                    } // end switch match
                } // end while 1

                break;
            } // end case 1

 
        } // end switch rule options 
    } // end while

    printf("c2_rule end_______%d\n",rule);
    return rule;
}

/**
 * @brief Function d_rule is rule D / <do> defined in grammar
 * @param fil Pointer to a file
 * @params L  Pointer to a list
 * @return    returns -1 if rule rule does not carries out
 *                    or ammount of used tokens in a rule
 **/
int d_rule(FILE *fil, tList *L)
{
    printf("d_rule start\n");
    int alt = START_CASE,                 /* rule option state */
        alt_bool = TRUE,                   /* use for iterate over rules */
        rule = 0;
    
    while (alt_bool) {
        switch (alt) {
            case START_CASE :    // D -> B D 
            {
                printf("D -> B D\n");
                int match = START_CASE, match_bool = TRUE;
                while (match_bool ) {
                    switch (match) {
                        case START_CASE: // B
                        {
                        	int items = b_rule( fil,L);
                            if ( items != MISSMATCH ) {
                            	rule += items;
                                match++;
                                printf("rule:%d\n",rule);
                            }
                            else {
                                match = MISSMATCH;
                            }
                            break;
                        }
                        case CASE_1: // D
                        {                 
                        	int items = d_rule( fil,L);
                            if ( items != MISSMATCH ) {
                                rule += items;
                                match_bool = FALSE;
                                alt_bool = FALSE;
                                printf("rule:%d\n",rule);
                            }
                            else {
                                match = MISSMATCH;
                            }
                            break;
                        }
                        default :
                        {
                        	move_back(L,rule);
                        	rule = 0;
                            match_bool = FALSE;
                            alt++;
                            break;
                        }

                    } // end switch match

                } // end while 

                break;

            }

            case CASE_1 : // D -> end EOL
            {
                printf("D -> end EOL\n");
                int match = START_CASE, match_bool = TRUE;
                while (match_bool ) {
                    switch (match) {
                        case START_CASE: // end
                        {
/* print */                	PrintAct(L); 
                            if (L->Act->dataList.type == T_END) {
                            	get_token(fil,L);
                            	rule++;
                                match++;
                            }
                            else {
                                match = MISSMATCH;
                            }
                            break;
                        }
                        case CASE_1: // EOL
                        {
/* print */                	PrintAct(L); 
                            if (L->Act->dataList.type == T_EOL) {
                            	get_token(fil,L);
                                alt_bool = FALSE;
                                match_bool = FALSE;
                                rule++;
                                printf("rule:%d\n",rule);
                            }
                            else {
                                match = MISSMATCH;
                            }
                            break;
                        }

                        default :
                        {
                        	move_back(L,rule);
                        	rule = -1;
                            match_bool = FALSE;
                            alt_bool = FALSE;
                            break;
                        }
                    } // end switch match
                } // end while 1

                break;
            } // end case 1
        } // end switch rule options 
    } // end while
    printf("d_rule end_______%d\n",rule);
    return rule;
}
/**
 * @brief Function e_rule is rule E / <func> defined in grammar
 * @param fil Pointer to a file
 * @params L  Pointer to a list
 * @return    returns -1 if rule rule does not carries out
 *                    or ammount of used tokens in a rule
 **/
int e_rule(FILE *fl,tList *L) {
    printf("e_rule start\n");
    int alt = START_CASE;
    int rule = 0;
    bool rulebool = TRUE; //false when rule check is over

    while (rulebool ){
        switch(alt) { //cases for all alternatives of this rule
            case START_CASE: { //E -> length ( G
                printf("E -> length ( G\n");
                int match = START_CASE;
                bool matchbool = true;

                while (matchbool){
                    switch(match) {
                        case START_CASE: { // length
/* print */                	PrintAct(L); 
                            if (L->Act->dataList.type == T_IDF)
                                if (strcmp(L->Act->dataList.data.str_cont, "length") == 0){
                                    get_token(fl,L);
                                    rule++;
                                    match++;
                                }
                                else
                                    match = MISSMATCH;
                            else
                            	match = MISSMATCH;
                            
                            break;
                        }

                        case CASE_1: { // (
/* print */                	PrintAct(L); 
                            if (L->Act->dataList.type == T_OpenBracket){
                            	get_token(fl,L);
                            	rule++;
                                match++;
                            }
                            else {
                                match = MISSMATCH;
                            }
                            break;
                        }

                        case CASE_2: // G
                        {
                            int items = g_rule(fl,L);
                            if ( items != MISSMATCH) {
                                rule+= items;  
                                matchbool = FALSE;
                                rulebool = FALSE;
                            }
                            else 
                                match = MISSMATCH;
                            break;
                        } 

                        default: {
                        	move_back(L,rule);
                        	rule =0;
                            alt++;
                            matchbool = false;
                            break;
                        }
                    }
                }
                break;
            }

            case CASE_1: { //E -> substr ( G
                printf("E -> substr ( G\n");
                int match = START_CASE;
                bool matchbool = true;

                while (matchbool){
                    switch(match) {
                        case START_CASE: { // substr
/* print */                	PrintAct(L); 
                            if (L->Act->dataList.type == T_IDF)
                                if (strcmp(L->Act->dataList.data.str_cont, "substr") == 0){
                                	get_token(fl,L);
                                	rule++;
                                    match++;
                                }
                                else{
                                    match = MISSMATCH;
                                }
                            else{
                                match = MISSMATCH;
                            }
                            break;
                        }

                        case CASE_1: { // (
/* print */                	PrintAct(L); 
                            if (L->Act->dataList.type == T_OpenBracket){
                            	get_token(fl,L);
                            	rule++;
                                match++;
                            }
                            else{
                            	match = MISSMATCH;
                            }
                            break;
                        }

                        case CASE_2: // G
                        {
                            int items = g_rule(fl,L);
                            if ( items != MISSMATCH) {
                                rule+= items;  
                                matchbool = FALSE;
                                rulebool = FALSE;
                            }
                            else 
                                match = MISSMATCH;
                            break;
                        } 

                        default: {
                        	move_back(L,rule);
                        	rule = 0;
                            alt++;
                            matchbool = false;
                            break;
                        }
                    }
                }
                break;
            }

            case CASE_2: { //E -> inputi () /inputs () /inputf () 
                printf("E -> inputx()\n");
                int match = START_CASE;
                bool matchbool = true;

                while (matchbool){
                    switch(match) {
                        case START_CASE: {                
/* print */                 PrintAct(L); 
                            if (L->Act->dataList.type == T_IDF){
                                if (strcmp(L->Act->dataList.data.str_cont, "inputi") == 0) {
                                    get_token(fl,L);
                                    rule++;
                                    match++;
                                }
                                else if (strcmp(L->Act->dataList.data.str_cont, "inputf") == 0) {
                                    get_token(fl,L);
                                    rule++;
                                    match++;
                                }
                                else if (strcmp(L->Act->dataList.data.str_cont, "inputs") == 0) {
                                    get_token(fl,L);
                                    rule++;
                                    match++;
                                }                                
                                else {
                                    match = MISSMATCH;
                                }
                            }
                            else {
                                match = MISSMATCH;
                            }
                            break;
                        }

                        case CASE_1:    // (
                        {
/* print */                 PrintAct(L); 
                            if (L->Act->dataList.type == T_OpenBracket){
                                get_token(fl,L);
                                rule++;
                                match++;
                            }
                            else{
                                match = MISSMATCH;
                            }
                            break;
                        }

                        case CASE_2:    // )
                        {
/* print */                 PrintAct(L); 
                            if (L->Act->dataList.type == T_ClosBracket){
                                get_token(fl,L);
                                rule++;
                                matchbool = FALSE;
                                rulebool = FALSE;
                            }
                            else{
                                match = MISSMATCH;
                            }
                            break;
                        }
                        default:
                        {
                            move_back(L,rule);
                            rule = 0;
                            alt++;
                            matchbool = FALSE;
                            break;
                        }
                    } // end switch
                } // end while 
                break;            
            } // end CASE 2

            case CASE_3: { //E -> ord ( G
                printf("E -> ord ( G\n");
                int match = START_CASE;
                bool matchbool = true;

                while (matchbool){
                    switch(match) {
                        case START_CASE: { // ord
/* print */                	PrintAct(L); 
                            if (L->Act->dataList.type == T_IDF)
                                if (strcmp(L->Act->dataList.data.str_cont, "ord") == 0){
                                	get_token(fl,L);
                                	rule++;
                                    match++;
                                }
                                else{
                                    match = MISSMATCH;
                                }
                            else{
                                match = MISSMATCH;
                            }
                            break;
                        }

                        case CASE_1: { // (
/* print */                	PrintAct(L); 
                            if (L->Act->dataList.type == T_OpenBracket){
                            	get_token(fl,L);
                            	rule++;
                                match++;
                            }
                            else{
                            	match = MISSMATCH;
                            }
                            break;
                        }

                        case CASE_2: // G
                        {
                            int items = g_rule(fl,L);
                            if ( items != MISSMATCH) {
                                rule+= items;  
                                matchbool = FALSE;
                                rulebool = FALSE;
                            }
                            else 
                                match = MISSMATCH;
                            break;
                        }

                        default: {
                        	move_back(L,rule);
                        	rule=0;
                            alt++;
                            matchbool = false;
                            break;
                        }
                    }
                }
                break;
            }

            case CASE_6: { //E -> chr ( G
                printf("E -> chr ( G\n");
                int match = START_CASE;
                bool matchbool = TRUE;

                while (matchbool==TRUE){
                    switch(match) {
                        case START_CASE: { // chr
/* print */                	PrintAct(L); 
                            if (L->Act->dataList.type == T_IDF)
                                if (strcmp(L->Act->dataList.data.str_cont, "chr") == 0){
                                	get_token(fl,L);
                                	rule++;
                                    match++;
                                printf("rule:%d\n",rule);
                                }
                                else{
                                    match = MISSMATCH;
                                }
                            else{
                                match = MISSMATCH;
                            }
                            break;
                        }

                        case CASE_1: { // (
/* print */                	PrintAct(L); 
                            if (L->Act->dataList.type == T_OpenBracket){
                            	get_token(fl,L);
                            	rule++;
                                match++;
                                printf("rule:%d\n",rule);
                            }
                            else{
                            	match = MISSMATCH;
                            }
                            break;
                        }

                        case CASE_2: // G
                        {
                            int items = g_rule(fl,L);
                            if ( items != MISSMATCH) {
                                rule+= items;  
                                matchbool = FALSE;
                                rulebool = FALSE;
                            }
                            else 
                                match = MISSMATCH;
                            break;
                        }

                        default: {
                        	move_back(L,rule);
                        	rule= MISSMATCH;
                            alt++;
                            matchbool = false;
                            break;
                        }
                    }
                }
                break;
            }

            default: {
            	rulebool = false;
                break;
            }
        }
    }
    printf("e_rule end_______%d\n",rule);
    return rule;
}
/**
 * @brief Function f_rule is rule F / <print> defined in grammar
 * @param fil Pointer to a file
 * @params L  Pointer to a list
 * @return    returns -1 if rule rule does not carries out
 *                    or ammount of used tokens in a rule
 **/
int f_rule(FILE *fl, tList *L) {
    printf("f_rule start\n");
    int rule = 0;
    int alt = START_CASE;
    bool rulebool = true; //false when rule check is over

    while (rulebool){
        switch(alt) { //cases for all alternatives of this rule
            case START_CASE: { //F -> ( G EOL
                printf("F -> ( G EOL\n");
                int match = START_CASE;
                bool matchbool = true;

                while (matchbool){
                    switch(match) {
                        // print bracket case
                        case START_CASE: { // (
/* print */                	PrintAct(L); 
                            if (L->Act->dataList.type == T_OpenBracket){
                            	get_token(fl,L);
                            	rule++;
                                match++;
                            }
                            else{
                                match = MISSMATCH;
                            }
                            break;
                        }

                        case CASE_1: { // G
                          	int items = g_rule(fl, L);
                            if (items != MISSMATCH){
                            	rule += items;
                                match++;
                                printf("rule:%d\n",rule);
                            }
                            else
                                match = MISSMATCH;
                            break;
                        }

                        case 2: { // EOL
/* print */                	PrintAct(L); 
                            if (L->Act->dataList.type == T_EOL) {
                               	get_token(fl,L);
                                rulebool = false;
                                matchbool = false;
                                rule++;
                            }
                            else{
                                match = MISSMATCH;
                            }
                            break;
                        }

                        default: {
                        	move_back(L,rule);
                        	rule = 0;
                            alt++;
                            matchbool = false;
                            break;
                        }
                    }
                }
                break;
            }

            case CASE_1: { //F -> term F2
                printf("F -> term F2\n");
                int match = START_CASE;
                bool matchbool = true;

                while (matchbool == TRUE){
                    switch(match) {
                        case 0: { // terms
                            PrintAct(L);
                            // in print only term can occur
                        	if ( check_term(L) ){
                                get_token(fl,L);
                                rule++;
                                match++;
                            }
                            else
                                match = MISSMATCH;
                            break;
                        }

                        case CASE_1: { // F2
                        	int items = f2_rule(fl, L);
                        	printf("items %d\n",items );
                            if (items != MISSMATCH) {
                                rulebool = FALSE;
                                matchbool = FALSE;
                                rule+=items;
                                printf("rule:%d\n",rule);
                            }
                            else{
                                match = MISSMATCH;
                            }

                            break;
                        }

                        default: {
                        	move_back(L,rule);
                        	rule = MISSMATCH;
                            alt++;
                            matchbool = false;
                            break;
                        }
                    }
                }
                break;
            }

            default: {
                rulebool = false;
                break;
            }
        }
    }
    printf("f_rule end_______%d\n",rule);
    return rule;
}

/**
 * @brief Function f2_rule is rule F2 / <print2> defined in grammar
 * @param fil Pointer to a file
 * @params L  Pointer to a list
 * @return    returns -1 if rule rule does not carries out
 *                    or ammount of used tokens in a rule
 *
 * Additional rule to rule <print>. Determines number of different items
 * used in the print function
 **/
int f2_rule(FILE *fl,tList *L){
    printf("f2_rule start\n");
    int rule = 0;
    int alt = START_CASE;
    bool rulebool = true; //false when rule check is over

    while (rulebool){
        switch(alt) { //cases for all alternatives of this rule
            case START_CASE: { //F2 -> , term F2
                printf("F2 -> , term F2\n");
                int match = START_CASE;
                bool matchbool = true;

                while (matchbool){
                    switch(match) {
                        case START_CASE: { // ,
/* print */                	PrintAct(L); 
                            if (L->Act->dataList.type == T_CLM){
                            	get_token(fl,L);
                            	rule++;
                                match++;
                            }
                            else{
                                match = MISSMATCH;
                            }
                            break;
                        }

                        case CASE_1: { // term
                          	PrintAct(L);
                            if ( check_term(L) ){
                                get_token(fl,L);
                            	rule++;
                                match++;
                            }
                            else
                                match = MISSMATCH;
                            break;
                        }

                        case CASE_2: { // F2
                          	int items = f2_rule(fl, L);
                            if (items != MISSMATCH){
                            	rulebool = false;
                                matchbool = false;
                            	rule += items;
                                match++;
                                printf("rule:%d\n",rule);
                            }
                            else{
                                match = MISSMATCH;
                            }
                            break;
                        }

                        default: {
                        	move_back(L,rule);
                        	rule = 0;
                            alt++;
                            matchbool = false;
                            break;
                        }
                    }
                }
                break;
            }

            case CASE_1: { //F2 -> EOL
            	printf("F2 -> EOL\n");
            	PrintAct(L);
                if (L->Act->dataList.type == T_EOL){
                	get_token(fl,L);
					rulebool = false;	
                	rule++;
                                printf("rule:%d\n",rule);
                }
                else{
                	rule = MISSMATCH;
                	alt++;
                }

                break;
            }

            default: {
                rulebool = false;
                break;
            }
        }
    }
    printf("f2_rule end_______%d\n",rule);
    return rule;	
}

/**
 * @brief Function g_rule is rule G / <it-list1> defined in grammar
 * @param fil Pointer to a file
 * @params L  Pointer to a list
 * @return    returns -1 if rule rule does not carries out
 *                    or ammount of used tokens in a rule
 * 
 * Accumulates items in function declaration or helps to acquire items 
 * in function print in case print starts with bracket
 **/
int g_rule(FILE *fl,tList *L) {
    printf("g_rule start\n");
    int rule = 0;
    int alt = START_CASE;
    bool rulebool = true; //false when rule check is over

    while (rulebool){
        switch(alt) { //cases for all alternatives of this rule
            case START_CASE: { //G -> term G2
                printf("G -> term G2\n");
                int match = START_CASE;
                bool matchbool = true;

                while (matchbool){
                    switch(match) {
                        case START_CASE: { // term
                            PrintAct(L);
                            if (check_term(L)){
                                get_token(fl,L);
                                rule++;
                                match++;
                            }
                            else
                                match = MISSMATCH;
                            break;
                        }

                        case CASE_1: { // G2
                        	int items = g2_rule(fl, L);
                            if (items != MISSMATCH){
                                rulebool = false;
                                matchbool = false;
                                rule+=items;
                                printf("rule:%d\n",rule);
                            }
                            else
                                match = MISSMATCH;
                            break;
                        }

                        default: {
                        	move_back(L,rule);
                        	rule = 0;
                            alt++;
                            matchbool = false;
                            break;
                        }
                    }
                }
                break;
            }

            case CASE_1: { //G -> )
                printf("G -> )\n");
/* print */     PrintAct(L); 
                if (L->Act->dataList.type == T_ClosBracket) {
                	get_token(fl,L);
                	rule++;
                    rulebool = false;
                                printf("rule:%d\n",rule);
                }
                else {
                	rule = MISSMATCH;
                    alt++;
                }
                break;
            }

            default: {
                rulebool = false;
                break;
            }

        }
    }
    printf("G_rule end_______%d\n",rule);
    return rule;
}

/**
 * @brief Function g2_rule is rule G2 / <it-list2> defined in grammar
 * @param fil Pointer to a file
 * @params L  Pointer to a list
 * @return    returns -1 if rule rule does not carries out
 *                    or ammount of used tokens in a rule
 *
 * Additional rule to G rule.
 **/
int g2_rule(FILE *fl,tList *L) {
    printf("G2_rule start\n");
    int rule = 0;
    int alt = START_CASE;
    bool rulebool = true; //false when rule check is over

    while (rulebool){
        switch(alt) { //cases for all alternatives of this rule
            case START_CASE: { //G2 -> , term G2
                printf("G2 -> , term G2\n");
                int match = START_CASE;
                bool matchbool = true;

                while (matchbool){
                    switch(match) {
                    	case START_CASE: { //G2 -> ,
/* print */ 			    PrintAct(L); 
			                if (L->Act->dataList.type == T_CLM) {
			                	get_token(fl,L);
			                	rule++;
                                match++;
			                }
			                else {
			                	match = MISSMATCH;
			                }
			                break;
			            }
                        case CASE_1: { // term
                            PrintAct(L);
                            if ( check_term(L) ){
                                get_token(fl,L);
                                rule++;
                                match++;
                            }
                            else
                                match = MISSMATCH;
                            break;
                        }

                        case CASE_2: { // G2
                        	int items = g2_rule(fl, L);
                            if (items != MISSMATCH){
                                rulebool = false;
                                matchbool = false;
                                rule+=items;
                                printf("rule:%d\n",rule);
                            }
                            else
                                match = MISSMATCH;
                            break;
                        }

                        default: {
                        	move_back(L,rule);
                        	rule = 0;
                            alt++;
                            matchbool = false;
                            break;
                        }
                    }
                }
                break;
            }

            case CASE_1: { //G2 -> )
                printf("G2 -> )\n");
/* print */     PrintAct(L); 
                if (L->Act->dataList.type == T_ClosBracket) {
                	get_token(fl,L);
                	rule++;
                    rulebool = false;
                                printf("rule:%d\n",rule);
                }
                else {
                	rule = MISSMATCH;
                    alt++;
                }
                break;
            }

            default: {
                rulebool = false;
                break;
            }

        }
    }

    printf("G2_rule end_______%d\n",rule);
    return rule;
}

/**
 * @brief Function h_rule is rule H / <asg> defined in grammar
 * @param fil Pointer to a file
 * @params L  Pointer to a list
 * @return    returns -1 if rule rule does not carries out
 *                    or ammount of used tokens in a rule
 *
 * Rule H determines correct construction of assignment operation
 **/
int h_rule(FILE *fl,tList *L) {
    printf("H_rule start\n");
    bool rulebool = true; //false when rule check is over
    int rule = 0;
    int alt = START_CASE;

    while (rulebool){
        switch(alt) { //cases for all alternatives of this rule
            case START_CASE: // E -> E EOL
            {
/* print */     printf("E -> E EOL\n");
                int match = START_CASE, match_bool = TRUE;
                while (match_bool ) {
                    switch (match) {
                        case START_CASE: // B
                        {
                            int items = e_rule(fl,L);
                            if ( items != MISSMATCH ) {
                                rule += items;
                                match++; 
                                printf("rule:%d\n",rule);
                            }
                            else {
                                match = MISSMATCH;
                            }

                            break;
                        }
                        case CASE_1: // EOL
                        {
/* print */                 PrintAct(L); 
                            if (L->Act->dataList.type == T_EOL) {
                                get_token(fl,L);
                                match_bool = FALSE;
                                rule++;
                                rulebool = FALSE;
                                printf("rule:%d\n",rule);
                            }
                            else {
                                match = MISSMATCH;
                            }
                            break;
                        }

                        default :
                        {
                            printf("default H->E EOL\n");
                            move_back(L,rule);
                            rule=0;
                            match_bool = FALSE;
                            alt++;
                            break;
                        }

                    } // end switch match
                } // end while 1

                break;
            } // end case 1
            

            case CASE_1: { //H -> id F
                printf("H -> id F\n");
                int match = START_CASE, match_bool = TRUE;
                while (match_bool ) {
                    switch (match) {
                        case START_CASE: // idf
                        {
/* print */                 PrintAct(L); 
                            if ( L->Act->dataList.type == T_IDF) {
                                get_token(fl,L);
                                rule++;
                                match++;
                                printf("rule:%d\n",rule);
                            }
                            else 
                                match = MISSMATCH;
                            
                            break;
                        }
                        
                        case CASE_1: // H
                        {
                            int items = f_rule(fl,L);
                            if ( items != MISSMATCH ) {
                                match_bool = FALSE;
                                rule += items;
                                rulebool = FALSE;
                              printf("rule:%d\n",rule);
                            }
                            else 
                                match = MISSMATCH;
                            break;
                        }

                        default :
                        {
                            move_back(L,rule);
                            rule= 0;
                            match_bool = FALSE;
                            alt++;
                            break;
                        }

                    } // end switch match
                }   // end while 

                break;
            }

            case CASE_2: { //H -> PSA EOL
                printf("H -> PSA EOL\n");
                int match = START_CASE;
                bool matchbool = true;
                PrintAct(L);
                if ( check_term(L) == FALSE ){
                    printf("Ack not term\n");
                    rule = MISSMATCH;
                    matchbool = FALSE;
                    rulebool = FALSE;
                }
                while (matchbool) {
                    switch(match) {
                        case START_CASE: { // psa
                            // trying to avoid psa check whether statement is of type id = term EOL
                            PrintAct(L);
                            get_token(fl,L);
                            PrintAct(L);
                            if (L->Act->dataList.type == T_EOL){
                                Pred(L);
                                PrintAct(L);
                                if ( check_term(L) ){
                                    Succ(L);
                                    PrintAct(L);
                                    get_token(fl,L);
                                    return 2;
                                }
                                // if there is not term before EOL, it is obvies, syntax err occured 
                                //if (err_code == NO_ERROR){
                                //   err_code = SYNTAX_ERROR;
                                //}
                                // returning unsucessful process of H rule
                                return MISSMATCH;
                            }
                            else if(L->Act->dataList.type == T_EOF){
                                match = MISSMATCH;
                            }
                            else{
                                Pred(L);
                                PrintAct(L);
                                printf("test PSA \n");
                                rule = Precedence_SA(fl,L);
                                printf("PSA returned: %d.\n Act after PSA ",rule );
                                PrintAct(L);
                                if (rule != MISSMATCH)
                                {
                                    match++;
                                }
                                else{
                                    rule = 0;
                                    match = MISSMATCH;
                                }
                            }
                            printf("H test\n");

                            break;
                        }

                        case 1: { // EOL
                            PrintAct(L);
                            if (L->Act->dataList.type == T_EOL) {
                                get_token(fl,L);
                                rulebool = false;
                                matchbool = false;
                                rule++;
                                printf("rule:%d\n",rule);
                            }
                            else
                                match = MISSMATCH;
                            break;
                        }

                        default: {
                            move_back(L,rule);
                            rule=MISSMATCH;
                            alt++;
                            matchbool = false;
                            break;
                        }

                    }
                }
                break;
            }

            default:
            {
                rulebool = FALSE;
            }
        }
    }
    printf("H_rule end_______%d\n",rule);
    return rule;
}

/**
 * @brief Function i_rule is rule I / <item> defined in grammar
 * @param fil Pointer to a file
 * @params L  Pointer to a list
 * @return    returns -1 if rule rule does not carries out
 *                    or ammount of used tokens in a rule
 *
 * Function launches Precedence analysis
 **/
int i_rule(FILE *fl, tList *L) {
    printf("PSA start\n");
    int rule = 0;

    rule = Precedence_SA(fl,L);
    
    printf("PSA end, returning:%d. \n",rule );
    return rule;
}
