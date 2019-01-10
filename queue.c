/***
 * @brief	 Queue for accumulating chars from source file.
 * @authors: Daniel Ocenas, xocena06
 *
 * Defines operations with queue for acquiring symbols from source file.
 */

#include "queue.h"

/**
 * @brief 	Function initiates queue
 * @param 	q Pointer to a queue
 * @return 	returns 1 if mallocation error
 *
 * Mallocs space for pointer to string, of DEFAULT_QLENGHT, 
 * sets presents size to DEFAULT
 * b_index indicates ammount of chars in queue
 **/
int queue_set (tQueue* q)
{

	q->arr = (char*) malloc(sizeof(char) *DEFAULT_QLENGHT);

	if ( q->arr == NULL)
	{
		return 1;
	}
	q->present_size = DEFAULT_QLENGHT;
	q->b_index = 0;
	q->arr[0] = '\0';	
	return 0;
}

/**
 * @brief 	Function adds char to a queue
 * @param 	q Pointer to a queue
 * @param 	c Char that needs is added to queue
 * @return 	Function returns 1 in case of mallocation error
 **/
int queueUp (tQueue* q, char c)
{
	if (q->b_index == q->present_size - 1 )
	{
		q->present_size ++;
		if ( (q->arr = (char*) realloc(q->arr,q->present_size) ) == NULL)
		{
			return 1;
		}
	}

	q->arr[q->b_index] = c;
	q->b_index++;
	q->arr[q->b_index] = '\0';
	
	return 0;
}

/**
 * @brief Function frees queue 
 * @param q Pointer to a queue
 */
void clear_queue(tQueue* q)			// clears queue -> set to intial value
{
	free(q->arr);
	q->b_index = 0;
}