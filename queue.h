/***
 * @brief	 Header file for queue
 * @author: Daniel Ocenas, xocena06
 *
 * Defines structure of buffer and function prototypes.
 */
#include "main.h"

#ifndef _QUEUE_H_
#define _QUEUE_H_

typedef struct {
    char *arr;                          /* pointer to char for storing symbols from imput */
    int b_index;                        /* index of char behind the last position */
    int present_size;					/* active size of queue */
} tQueue;

#define DEFAULT_QLENGHT		2			

int queue_set (tQueue* q);
int queueUp (tQueue* q, char c);
void clear_queue(tQueue* q);

#endif //_QUEUE_H_