/***
 * Projekt IFJ2018
 * @brief       Header file for parser
 * @authors:    Daniel Miloslav Očenáš, xocena06
 *              Michal Vanka,           xvanka00
 *
 * Defines function prototypes and operations performed during syntax analysis
 */

#ifndef PROJIFJ_SYNT_A_H
#define PROJIFJ_SYNT_A_H


#include "lex_a.h"
#include "list.h"
#include "symtable.h"
#include "psa.h"
#include "errors.h"

#define MISSMATCH 		-1 
#define START_CASE	 	0
#define CASE_1			1
#define CASE_2			2
#define CASE_3			3
#define CASE_4			4
#define CASE_5			5
#define CASE_6			6
#define CASE_7			7
extern int err_code;		/* Variable int err_code. Global variable for errors */

/**
 * Function prototypes
 **/
void list_to_symtable(tList *L, int res);
void delete_token(Token *token);
void get_token(FILE *fil, tList *L);
void print_list(tList *L);
void PrintAct(tList *L);
bool check_term(tList *L);
int a_rule (FILE *fil, tList *L);
int b_rule (FILE *fil, tList *L);
int c_rule (FILE *fil, tList *L);
int c2_rule(FILE *fil, tList *L);
int d_rule (FILE *fil, tList *L);
int e_rule (FILE *fl, tList *L);
int f_rule (FILE *fl, tList *L);
int f2_rule(FILE *fl, tList *L);
int g_rule (FILE *fl, tList *L);
int g2_rule(FILE *fl, tList *L);
int h_rule (FILE *fl, tList *L);
int i_rule (FILE *fl, tList *L);
int synt_a (char *filename);

#endif //PROJIFJ_SYNT_A_H
