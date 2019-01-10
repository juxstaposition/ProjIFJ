/***
 * Project IFJ18
 * @name Precedence Syntax Analysis
 * @authors Romana Dzubarova (xdzuba00), Michal Vanka (xvanka00)
 */

#include "psa.h"

#define TABLE_INDEX_1 0
#define TABLE_INDEX_2 1
#define TABLE_INDEX_3 2
#define TABLE_INDEX_4 3
#define TABLE_INDEX_5 4
#define TABLE_INDEX_6 5
#define TABLE_INDEX_7 6
#define TABLE_INDEX_8 7
#define TABLE_INDEX_9 8
#define TABLE_INDEX_10 9

#define TABLE_SIZE 10

int precedence_table[TABLE_SIZE][TABLE_SIZE] =

        {///                 ENTER SYMBOL
        ///      | + | - | * | / | c | r | ( | ) | i | $ | /// STACK
                { L , L , G , G , L , L , G , L , G , L }, /// +
                { L , L , G , G , L , L , G , L , G , L }, /// -
                { L , L , L , L , L , L , G , L , G , L }, /// *
                { L , L , L , L , L , L , G , L , G , L }, /// /
                { G , G , G , G , N , G , G , L , G , L }, /// c (comparative operators) == !=
                { G , G , G , G , L , N , G , L , G , L }, /// r (relation operators) < <= > >=
                { G , G , G , G , G , G , G , E , G , N }, /// (
                { L , L , L , L , L , L , N , L , N , L }, /// )
                { L , L , L , L , L , L , N , L , N , L }, /// i
                { G , G , G , G , G , G , G , N , G , N }, /// $

        };

/***
 * This function detects index in table from symbol
 * @param SYMBOL - which represents block in table
 * @return index in precedence table
 */
int PRECEDENCE_TABLE_INDEX (precedence_table_symbol SYMBOL)
{
    switch (SYMBOL)
    {
        case PLS:
            return TABLE_INDEX_1;

        case MIN:
            return TABLE_INDEX_2;

        case MUL:
            return TABLE_INDEX_3;

        case DIV:
            return TABLE_INDEX_4;

        case EQL:
        case NQE:
            return TABLE_INDEX_5;

        case LES:
        case LEQ:
        case MOR:
        case MRE:
            return TABLE_INDEX_6;

        case OpenBracket:
            return TABLE_INDEX_7;

        case ClosBracket:
            return TABLE_INDEX_8;

        case ID:
        case INT:
        case DOUBLE:
        case STRING:
            return TABLE_INDEX_9;

        default:
            return TABLE_INDEX_10;
    }
}

/***
 * This function detects symbol from token
 * @param token - this is what we get from syntax analysis
 * @return - symbol, which represent block in table
 */
static precedence_table_symbol SYMBOL_FROM_TOKEN (Token* token) //berie token vracia znak
{
    switch (token->type)
    {
        case T_PLS:
            return PLS;
        case T_MIN:
            return MIN;
        case T_MUL:
            return MUL;
        case T_DIV:
            return DIV;
        case T_OpenBracket:
            return OpenBracket;
        case T_ClosBracket:
            return ClosBracket;
        case T_LES:
            return LES;
        case T_LEQ:
            return LEQ;
        case T_MOR:
            return MOR;
        case T_MRE:
            return MRE;
        case T_EQL:
            return EQL;
        case T_NQE:
            return NQE;
        case T_IDF:
            return ID;
        case T_INT:
            return INT;
        case T_DOUBLE:
            return DOUBLE;
        case T_STR:
            return STRING;
        default:
            return DOLLAR;
    }
}

/***
 * This function push symbol on top of the Stack
 * @param symbol - symbol, which represent token
 * @param Stack - name of stack where we get symbol
 * @return - if is something wrong returns -1 else returns 0
 */
int Stack_symbol_push(precedence_table_symbol symbol, Stack_symbol* Stack)
{
    Stack_section_i* H_item = malloc(sizeof(struct Stack_section));

    if (H_item == NULL)
    {
        err_code = INTERNAL_ERROR;
        return -1;
    }

    H_item->item = symbol;
    H_item->next_item = Stack->stack_top;

    Stack->stack_top = H_item;

    return 0;
}

/***
 * This function pop symbol from top of Stack
 * @param Stack - name of stack where are symbols
 */
void Stack_symbol_pop (Stack_symbol* Stack)
{
    Stack_section_i* H_item;

    if (Stack->stack_top != NULL)
    {
        H_item = Stack->stack_top;
        Stack->stack_top = H_item->next_item;
        free(H_item);
    }
}


int Stack_symbol_pop_r (Stack_symbol* Stack) //popujeme hodnotu zo zasobnika, ktora je v tom zasobniku ulozena na vrchole
{
    Stack_section_i* H_item;
    int p_Value = 0;

    if (Stack->stack_top != NULL)
    {
        H_item = Stack->stack_top;
        p_Value = H_item->item;
        Stack->stack_top = H_item->next_item;
        free(H_item);
    }

    return p_Value;
}

/**
 * When is on the top of stack NON_TERM(E) or ZNK(<), top of the stack is next symbol
 * @param Stack - name of stack where are symbols
 * @return top of the stack
 */
Stack_section_i* Stack_item_on_top (Stack_symbol* Stack)
{
    Stack_section_i* H_stack = Stack->stack_top;

     if (H_stack->item == NON_TERM || (H_stack->item == ZNK))
     {
         return H_stack->next_item;
     }
     else
     {
         return H_stack;
     }
}

/***
 * This function is popping Stack1 on the end of program or if there is mistake
 * @param Stack1 - name of Stack1
 */
void clean_up (Stack_symbol* Stack1)
{
    while (Stack1->stack_top->next_item != NULL)
    {
        Stack_symbol_pop(Stack1);
    }
}
/**
 * When we want to find out how many symbols was before ZNK(<)
 * @param Stack - name of stack
 * @return count of symbols before ZNK(<)
 */
int Count_of_symbol_before_ZNK (Stack_symbol* Stack)//function witch wants to return size of stack (size of expression?)
{
    Stack_section_i* H_Stack = Stack -> stack_top;
    int size = 0;

    while ( H_Stack->next_item != NULL)
    {
        if (H_Stack->item == ZNK)
        {
            break;
        }
        else
        {
            size += 1;
        }

        if (H_Stack->next_item->next_item->item == DOLLAR)
        {
            return size;
        }

        H_Stack = H_Stack->next_item;
    }

    return size;
}

/***
 * If we want to give some symbol before other symbol on the stack we use this function.
 * When on the top of the stack is NON_TERM(E), we give symbol before next symbol from top symbol.
 * @param Stack - name of stack
 * @param symbol - symbol which we get before other symbol
 * @return if there is mistake we return -1 else return 0
 */
int Give_symbol_before_top (Stack_symbol* Stack, precedence_table_symbol symbol)
{
    Stack_section_i* Top_symbol = Stack_item_on_top(Stack);
    Stack_section_i* New_Stack = malloc(sizeof(struct Stack_section));

    if (New_Stack == NULL)
    {
        if (err_code == NO_ERROR){
            err_code = INTERNAL_ERROR;
        }
        return -1;
    }

    New_Stack->item = symbol;
    
    if (Top_symbol == NULL)
    {
        New_Stack->next_item = Stack->stack_top;
        Stack->stack_top = New_Stack;
    }
    else if (Stack->stack_top->item == NON_TERM){
        New_Stack->next_item = Top_symbol;
        Stack->stack_top->next_item = New_Stack;
    }
    else{
        New_Stack->next_item = Top_symbol;
        Stack->stack_top = New_Stack;
    }
    return 0;
}

/**
 * This function detects which of rules was use in Precedence analysis.
 * In first case are rules E->i, in second are other rules.
 * @param Stack - name of symbol
 * @param count - count of symbol before <
 * @return rule which was use
 */
parser_rules reduce_expression_by_rules (Stack_symbol* Stack, int count)
{
    Stack_section_i* H_Stack_1 = NULL;
    Stack_section_i* H_Stack_2 = NULL;
    Stack_section_i* H_Stack_3 = NULL;

    switch (count)
    {
        case 1 :
            H_Stack_1 = Stack->stack_top;

            if ((H_Stack_1->item == ID) || (H_Stack_1->item == INT) || (H_Stack_1->item == DOUBLE) || (H_Stack_1->item == STRING))
            {
                return R_OPERAND;
            }
            else {
                return R_NOTR;
            }

        case 3 :
            H_Stack_1 = Stack->stack_top;
            H_Stack_2 = Stack->stack_top->next_item;
            H_Stack_3 = Stack->stack_top->next_item->next_item;

            if ((H_Stack_1->item == OpenBracket) && (H_Stack_2->item == NON_TERM) && (H_Stack_3->item == ClosBracket))
            {
                return R_BRACKET;
            }
            else if ((H_Stack_1->item == NON_TERM) && (H_Stack_3->item == NON_TERM))
            {
                if (H_Stack_2->item == PLS)
                {
                    return R_PLS;
                }
                if (H_Stack_2->item == MIN)
                {
                    return R_MIN;
                }
                if (H_Stack_2->item == MUL)
                {
                    return R_MUL;
                }
                if (H_Stack_2->item == DIV)
                {
                    return R_DIV;
                }
                if (H_Stack_2->item == LES)
                {
                    return R_LES;
                }
                if (H_Stack_2->item == LEQ)
                {
                    return R_LEQ;
                }
                if (H_Stack_2->item == MOR)
                {
                    return R_MOR;
                }
                if (H_Stack_2->item == MRE)
                {
                    return R_MRE;
                }
                if (H_Stack_2->item == EQL)
                {
                    return R_EQL;
                }
                if (H_Stack_2->item == NQE)
                {
                    return R_NQE;
                } else
                {
                    return R_NOTR;
                }
            }
        default :
            return R_NOTR;
    }
}

/*int check_data_type (parser_rules rule, Stack_symbol* Stack, term_type data_type)
{
    Stack_section_i* H_Stack_1 = Stack->stack_top;
    Stack_section_i* H_Stack_2 = Stack->stack_top->next_item;
    Stack_section_i* H_Stack_3 = Stack->stack_top->next_item->next_item;

    if ((rule != R_BRACKET) || (rule != R_OPERAND)) //if it is not E->i or E-> (E)
    {
        if ((H_Stack_1->type_item == ID_UNDEF) || (H_Stack_3->type_item == ID_UNDEF)) //E is undefined
        {
            return 3;
        }

        if ((H_Stack_1->type_item == ID_BOOL) || (H_Stack_3->type_item == ID_BOOL)) // E is wrong data type
        {
            return 4;
        }

    }
    switch (rule) {
        case R_OPERAND: //E->i
        {
            if (H_Stack_1->type_item == ID_BOOL) //if i is wrong data type
            {
                return 4;//error 4
            }
            if (H_Stack_1->type_item == ID_UNDEF) //if i is undefined
            {
                return 3;//error 3
            }
        }

        case R_BRACKET: //E->(E)
        {
            if (H_Stack_2->type_item == ID_BOOL) //E is wrong data type
            {
                return 4;
            }
            if (H_Stack_2->type_item == ID_UNDEF) //E is undefined
            {
                return 3;
            }
        }

        case R_PLS:
        case R_MIN:
        case R_MUL:
            if ((H_Stack_1->type_item == ID_INT) && (H_Stack_3->type_item == ID_INT)) //if both of E are integers final type is integer
            {
                data_type = ID_INT;
                break;
            }

            if ((H_Stack_1->type_item == ID_STRING) && (H_Stack_3->type_item == ID_STRING) && (rule == R_PLS)) //concatenation if E->E + E, final type is string
            {
                data_type = ID_STRING;
                break;
            }

            if ((H_Stack_1->type_item == ID_STRING) || (H_Stack_3->type_item == ID_STRING)) //E - E or E * E can`t be string
            {
                return 4;
            }

            if((H_Stack_1->type_item == ID_INT) && (H_Stack_3->type_item == ID_DOUBLE)) //if first operand is float, first operand will become float and final data type is float
            {
                H_Stack_1->type_item = ID_DOUBLE;
                data_type = ID_DOUBLE;
            }

            else if((H_Stack_1->type_item == ID_DOUBLE) && (H_Stack_3->type_item == ID_INT)) //if 3. operand is float, 3. operand will become float and final data type is float
            {
                H_Stack_3->type_item = ID_DOUBLE;
                data_type = ID_DOUBLE;
            }

            else if((H_Stack_1->type_item == ID_DOUBLE) && (H_Stack_3->type_item == ID_DOUBLE)) //if both operands are float final data type is float
            {
                data_type = ID_DOUBLE;
            }
            break;

        case R_DIV:
            if (H_Stack_3 == '0') //docasne riesenie ak je delenie 0 dochadza k chzbe 9
            {
                return 9;
            }
            if ((H_Stack_1->type_item == ID_INT) && (H_Stack_3->type_item == ID_INT)) //if both operands are int final data type is int
            {
                data_type = ID_INT;
                break;
            }
            if ((H_Stack_1->type_item == ID_DOUBLE) || (H_Stack_3->type_item == ID_DOUBLE)) //if one of operand or both are float final operand is float
            {
                data_type = ID_DOUBLE;
                break;
            }
            if ((H_Stack_1->type_item == ID_STRING) || (H_Stack_3->type_item == ID_STRING)) //div string operands returns error
            {
                return 4;
            }
        case R_LES:
        case R_LEQ:
        case R_MOR:
        case R_MRE:
        case R_EQL:
        case R_NQE:
            data_type = ID_BOOL; //result of binary operations are bool data type

            if((H_Stack_1->type_item == ID_INT) && (H_Stack_3->type_item == ID_DOUBLE)) //if one of operand is float then this operand become float
            {
                H_Stack_1->type_item = ID_DOUBLE;
            }

            else if((H_Stack_1->type_item == ID_DOUBLE) && (H_Stack_3->type_item == ID_INT))
            {
                H_Stack_3->type_item = ID_DOUBLE;
            }

            break;

        default:
            break;
    }

}*/
/**
 * This function exchange symbols before < to NON_TERM (E)
 * @param Stack - name of stack
 * @return witch rule was used, if there is mistake return -1 else 0
 */
int Final_Reduce (Stack_symbol* Stack)
{
    int count = Count_of_symbol_before_ZNK(Stack);
    int rule = 0;
    int e=0;

    switch (count + 1)
    {
        case 2 :
            rule = reduce_expression_by_rules(Stack,count);
            if (rule == R_NOTR) {
                err_code = SYNTAX_ERROR;
                return -1;
            }

            for (int i = 1; i <= count + 1 ; i++)
            {
              Stack_symbol_pop(Stack);
            }

            e = Stack_symbol_push(NON_TERM,Stack);
            if (e == -1) {
                return -1;
            }
            return rule;

        case 4 :

            rule = reduce_expression_by_rules(Stack,count);
            if (rule == R_NOTR) {
                err_code = SYNTAX_ERROR;
                return -1;
            }

            for (int i = 1; i <= count + 1 ; i++)
            {
                Stack_symbol_pop(Stack);
            }

            e = Stack_symbol_push(NON_TERM,Stack);
            if (e == -1) {
                return -1;
            }

            return rule;

        default :

            return 0;

    }
}

/***
 * In this function we do final precedence syntax analysis. Function take tokens from List Ls and detects which type
 * of token is it and what type of symbol it is in precedence table. Then look into precedence table and returns symbol(E,G,L,N).
 * Next do operations based on returns symbol.
 * @param fl - name of file
 * @param Ls - name of list where are tokens
 * @return - count of token or -1 if there is mistake
 */
int Precedence_SA (FILE* fl, tList* Ls)
{

    Stack_symbol* stack = malloc(sizeof(Stack_symbol));

    if (stack == NULL)
    {
        if (err_code == NO_ERROR){
            err_code = INTERNAL_ERROR;
        }
        return -1;
    }

    stack->stack_top = NULL;

    int i = 0;
    int token_c = 0;

    int e = Stack_symbol_push(DOLLAR,stack);
    if (e == -1) {
        for (int k = 0; k < token_c; k++)
        {
            Pred(Ls);
        }
        clean_up(stack);
        return -1;
    }

    Stack_section_i* stack_symbol = malloc(sizeof(struct Stack_section));

    if (stack_symbol == NULL)
    {
        for (int k = 0; k < token_c; k++)
        {
            Pred(Ls);
        }
        if (err_code == NO_ERROR){
            err_code = INTERNAL_ERROR;
        }
        clean_up(stack);
        return -1;
    }

    precedence_table_symbol enter_symbol = 0;

    stack_symbol = Stack_item_on_top(stack);
    enter_symbol = SYMBOL_FROM_TOKEN(&Ls->Act->dataList);
    get_token(fl,Ls);

    while ((stack_symbol->item != DOLLAR) || (enter_symbol != DOLLAR))
    {
        switch (precedence_table[PRECEDENCE_TABLE_INDEX(stack_symbol->item)][PRECEDENCE_TABLE_INDEX(enter_symbol)])
        {
            case E :
                PrintAct(Ls);
                token_c++;
                e = Stack_symbol_push(enter_symbol,stack);
                if (e == -1) {
                    break;
                }
                enter_symbol = SYMBOL_FROM_TOKEN(&Ls->Act->dataList);
                stack_symbol = stack->stack_top;
                
                if(Ls->Act->dataList.type != T_EOL && Ls->Act->dataList.type != T_THEN && 
                   Ls->Act->dataList.type != T_DO  && Ls->Act->dataList.type != T_CLM){
                    get_token(fl,Ls);
                }
                break;

            case G:
                token_c++;
                Give_symbol_before_top(stack,ZNK);
                e = Stack_symbol_push(enter_symbol,stack);
                if (e == -1) {
                    break;
                }
                PrintAct(Ls);
                enter_symbol = SYMBOL_FROM_TOKEN(&Ls->Act->dataList);
                stack_symbol = stack->stack_top;

                if(Ls->Act->dataList.type != T_EOL && Ls->Act->dataList.type != T_THEN && 
                   Ls->Act->dataList.type != T_DO  && Ls->Act->dataList.type != T_CLM){
                    get_token(fl,Ls);
                }
                break;

            case L :
                e = Final_Reduce(stack);
                if (e == -1) {
                    break;
                }
                i = i + 1;
                stack_symbol = Stack_item_on_top(stack);
                break;

            case N :
                err_code = SYNTAX_ERROR;
                break;

            default:
                err_code = SYNTAX_ERROR;
                break;
        }

        if (err_code != 0)
        {
            for (int k = 0; k < token_c; k++)
            {
                Pred(Ls);
            }
            clean_up(stack);
            break;
        }
    }

    if (err_code != 0)
    {
        return -1;
    }

    free(stack_symbol);
    free(stack);
    return token_c;
}
