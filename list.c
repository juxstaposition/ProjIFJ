/***
 * Projekt IFJ2018
 * @brief	List for token storage
 * @authors: Daniel Ocenas, xocena06
 *
 * Defines operations with Dlist
 * Structure of these operations is based on the First project in the class Algorithms
 */


#include "list.h"


/**
 * @brief Prints error statement 
 **/
void Error() {	
    fprintf (stderr,"*ERROR* The program has performed illegar operation inside List.\n");
    return;
}
/**
 * @brief 	List initiation to default values
 * @param L Pointer to the List L
 *
 * Function is called before first usage of the list
 **/
void InitList (tList *L) {
    
	L->First = NULL;
	L->Act = NULL;
	L->Last = NULL;
	L->elem_count = 0;

}

/**
 * @brief Deletes all elements of the list and sets the List to the Initial state
 * @param L Pointer to the List L
 *
 * Correctly frees all elemtns of the List L
 **/
void DisposeList (tList *L) {

	while(L->First != NULL)		//cyklus prechadza zoznamom od 1. polozky, kym sa nedostane na poslednu, ktora odkazuje na NULL
	{
		tElemPtr disposer;										//pomoc na uvolnenie pamati jednotlivych prvkov
		disposer = L->First;										//nastavim na prvu polozku
		L->First = L->First->lptr;									//ukazatel na prvu polozku nastavim na nasledujucu
		if (  disposer->dataList.type == T_IDF || disposer->dataList.type == T_STR)
		{
			free(disposer->dataList.data.str_cont);
		}		
		free(disposer);												//mazem prvu polozku v zozname
	}	

	//nastavim zoznam ako po inicializaci
	L->Last = NULL;
	L->Act = NULL;
	L->First = NULL;
	L->elem_count = 0;
}

/**
 * @brief Inserts element to the begining of the List
 * @param L 	Pointer to the List L
 * @param val 	Token data to be inserted into element
 **/
void InsertFirst (tList *L, Token val) {

	tElemPtr new_first;											//vytvorenie novej polozky obojsmerne viazaneho zoznamu
	if ((new_first = malloc(sizeof(struct tElem))) == NULL)		//alokujem miesto, ak nieje pridelena pamat,
	{															//tak volam funkciu DLError(), ktora hlasi chybu
		Error();												//a funkciu ukoncim
		return;
	}

	L->elem_count++;
	new_first->rptr = L->First;									//ukazatel na prvu polozku pridelim ukazatelu na nasledujucu polozku novej polozky
	new_first->lptr = NULL;										//ukazatel na predchadzajucu polozku nastavime na nedefinovanu hodnotu
	new_first->dataList = val;									//pridelim data novej polozke obojmerne viazaneho zoznamu podla parametru funkcie val

	if (L->First != NULL)										//podmienka pre prikaz ktory potrebujem v pripade, ze zoznam uz obsahuje aspon 1 polozku
	{
		L->First->lptr = new_first;								//nasledujucu polozku prepojim s novou vlozenou na prve miesto
	}

	if(L->Last == NULL)												//ak este nebol prideleny posledny prvok zoznamu, nastavim ho pri vkladani prveho prvku
	{
		L->Last = new_first;		
	}

	L->First = new_first;											//odkaz na prvu polozku nastavim na novu polozku

}
/**
 * @brief Sets actitivty of the List L to the first element of the List L
 * @param L Pointer to the List L
 **/
void First (tList *L) {

	L->Act = L->First;

}

/**
 * @brief Sets actitivty of the List L to the last element of the List L
 * @param L Pointer to the List L
 **/
void Last (tList *L) {
	
	L->Act = L->Last;

}

/**
 * @brief Deletes first element of the List
 * @param L Pointer to the List L
 */
void DeleteFirst (tList *L) {

	if (L->First == NULL)											//ak je zoznam prazdny nic sa nedeje
	{
		return;
	}

	if (L->First == L->Act)											//ak je prva polozka aktivna, tak sa aktivita straca
	{
		L->Act = NULL;
	}

	tElemPtr delete_DLfirst = L->First;
	L->elem_count--;
	if (L->First != L->Last)										//ak nebola prva polozka zaroven posledna, nastavim ukazatele
	{																//podla vyznameveho prepojenia. Prvou polozkou zoznamu sa stava nasledujuca
		L->First = delete_DLfirst->rptr;							//v doteraz nasledujucej polozke potrebujem zmazat ukazatel na polozku, kt. idem mazat
		L->First->lptr = NULL;
	}
	else															//v pripade ze prvy prvok bol zaroven posledny, prvy prvok sa vymaze
	{																//a odkazy na prvu a poslednu polozku nastavujem na nedefinovane
		L->Last = NULL;
		L->First = NULL;
	}

	if (  delete_DLfirst->dataList.type == T_IDF || delete_DLfirst->dataList.type == T_STR)
	{
		free(delete_DLfirst->dataList.data.str_cont);
	}

	free(delete_DLfirst);

}	


/**
 * @brief Inserts element behind Active element of the List L
 * @param L 	Pointer to the List L
 * @param val 	Token data to be inserted into element
 **/
void PostInsert (tList *L, Token val) {
	
	if (L->Act == NULL)
	{
		return;
	}

	tElemPtr new_post;											//vytvorenie novej polozky obojsmerne viazaneho zoznamu vkladanej za aktivny prvok
	if ((new_post = malloc(sizeof(struct tElem))) == NULL)		//alokujem miesto, ak nieje prvku pridelena pamat,
	{																//tak volam funkciu DLError(), ktora hlasi chybu
		Error();													//a funkciu ukoncim
		return;
	}

	L->elem_count++;
	new_post->dataList = val;
	new_post->lptr = L->Act;
	new_post->rptr = L->Act->rptr;
	if (L->Act->rptr != NULL)
		L->Act->rptr->lptr = new_post;
	L->Act->rptr = new_post;
	if (L->Act == L->Last)
	{
		L->Last = new_post;
	}

}

/**
 * @brief Moves activity of the List L to the next element  
 * @param L 	Pointer to the List L
 **/     
void Succ (tList *L) {
	
	if (L->Act != NULL)
	{
		L->Act = L->Act->rptr;
	}

}

/**
 * @brief Moves activity of the List L to the previous element  
 * @param L 	Pointer to the List L
 **/ 
void Pred (tList *L) {
	
	if (L->Act != NULL || L->Act != L->First)
	{
		L->Act = L->Act->lptr;
	}

}
