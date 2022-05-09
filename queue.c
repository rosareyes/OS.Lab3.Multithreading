#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <pthread.h>
#include "queue.h"


// To create a queue
queue *queue_init(int num_elements)
{
        /*Initilize queue*/
        queue *q = (queue *)malloc(sizeof(queue));
        q->elem = (struct element*)malloc(sizeof(struct element) * num_elements);


        /*Checks if the queue is null and if the num_elements in negative or 0*/
         if (q == NULL)
        {
                return NULL;
        }
        if (num_elements <= 0)
        {
                num_elements = 10000;
        }

        q->max_elements = num_elements;
        q->current_size = 0;
        q->head = 0;
        q->tail = 0;
        return q;
}

// To Enqueue an element
int queue_put(queue *q, struct element* x)
{
        /*This operation is done in the tail of the queue*/
        if (queue_full(q) == 1)
        {
                printf("the queue is full right now\n");
        }
        else
        {
                q->elem[q->tail] =* x;
                q->current_size++;
                q->tail = q->tail + 1;
                /* As we fill the queue in circular fashion */
                if (q->tail == q->max_elements)
                {
                        q->tail = 0;
                }
        }
        return 0;
}

// To Dequeue an element.
struct element *queue_get(queue *q)
{
        struct element* element;

        /* If Queue size is zero then it is empty. So we cannot pop */
        if (queue_empty(q) == 1)
        {
                printf("the queue is empty right now\n");
        }
        /* Removing an element is equivalent to incrementing index of front by one */
        else
        {
                element = &(q->elem[q->head]);
                q->current_size--;
                q->head++;
                /* As we fill elements in circular fashion */
                if (q->head == q->max_elements)
                {
                        q->head = 0;
                }

        }
        return element;
}

// To check queue state
int queue_empty(queue *q)
{

        if (q->current_size == 0)
        {
                return 1;
        }
        
                return 0;
        
}

int queue_full(queue *q)
{
        if (q->current_size == q->max_elements)
        {
                return 1;
        }
               return 0;
        
}

// To destroy the queue and free the resources
int queue_destroy(queue *q)
{
        //free(q->elem);
        free(q);
        return 0;
}