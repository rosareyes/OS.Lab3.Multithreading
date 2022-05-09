/*=============================================================================
 |   Assignment:  MULTITHREADING
 |
 |       Author:  ROSA REYES (100434072)
 |				  MARIA SOLANA (100451095)
 |				  INES SANZ (100451104)
 |
 |     Language:  C
 |
 |       Course:  Operating Systems - G88 - 2021-2022
 |     Due Date:  09/05/2022
 |
 +-----------------------------------------------------------------------------
 |
 |  Description:  PROGRAMMING A COST CONTROL SYSTEM WITH A N-PRODUCERS/N-CONSUMERS
 |				  PROCESS.
 |
 *===========================================================================*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <stddef.h>
#include <sys/stat.h>
#include <pthread.h>
#include "queue.h"
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>

/**
 * Entry point
 * @param argc
 * @param argv
 * @return
 */

/*-------------Struct params for: the producers and consumers threads-------------*/
struct parameters
{
    int initial;
    int final;
    struct element *elem;
};

/*-------------------------------Global Variables-------------------------------*/
int has_started = 0;
int numInstruc;
int total = 0;
int finished = 0;
int instrucs_counter = 0;

queue *q;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t started;
pthread_cond_t no_full = PTHREAD_COND_INITIALIZER;
pthread_cond_t no_empty = PTHREAD_COND_INITIALIZER;

/*-------------------------------Producer Function-------------------------------*/
/**
 * It creates threads and produces information elements
 * @param void *p - parameter
 *
 */
void *producer(void *p)
{
    /*Copy in a local variable all the positions for the parameters*/
    struct parameters *param = (struct parameters *)p;
    memcpy(&param, &p, sizeof(p));

    /*We start the producer thread*/
    pthread_mutex_lock(&mutex);
    has_started = 1;
    pthread_cond_signal(&started);
    pthread_mutex_unlock(&mutex);
    int s = param->initial;

    /*Start to produce*/
    for (s; s <= param->final; s++)
    {
        /*Enters the critical zone if it is not empty*/
        pthread_mutex_lock(&mutex);
        while (queue_full(q))
        {
            pthread_cond_wait(&no_full, &mutex);
        }

        /*Puts the element in the queue(buffer)*/
        queue_put(q, &param->elem[s]);
        pthread_cond_signal(&no_empty);
        pthread_mutex_unlock(&mutex);
    }

    /*End of producer*/
    pthread_exit(0);
    return NULL;
}

/*-------------------------------Consumer Function-------------------------------*/
/**
 * It creates threads and consumes information from elements in the buffer (queue)
 * @param void *param - parameter
 *
 */
void *consumer(void *param)
{
    /*Saves the total sum of the cost*/
    int sum = 0;

    /*Saves the element consumed by the buffer (queue)*/
    struct element *element = (struct element *)malloc(sizeof(struct element));

    /*We start the consumer*/
    pthread_mutex_lock(&mutex);
    has_started = 1;
    pthread_cond_signal(&started);
    pthread_mutex_unlock(&mutex);

    /*We start to consume*/
    for (int i = 0;; i++)
    {

        /*Enters the critical zone if it is not empty*/
        pthread_mutex_lock(&mutex);
        while (queue_empty(q))
        {

            /*It's empty and we are done processing all the instructions*/
            if (instrucs_counter == numInstruc)
            {
                /*Once we are done, we use broadcast to wake up the last consumer and finish the program*/
                pthread_cond_broadcast(&no_empty);
                pthread_mutex_unlock(&mutex);
                pthread_exit((void *)(intptr_t)sum);
            }
            /*It's empty but we are not done -- go to sleep*/
            pthread_cond_wait(&no_empty, &mutex);
        }

        /*We consume the element, take it out from the buffer (queue) and add it to sum*/
        element = queue_get(q);
        instrucs_counter = instrucs_counter + 1;
        /*Exit the critical zone*/
        pthread_cond_signal(&no_full);
        switch (element->type)
        {
        case 1:
            sum += 3 * element->time;
            break;
        case 2:
            sum += 6 * element->time;
            break;
        case 3:
            sum += 15 * element->time;
            break;
        }

        pthread_mutex_unlock(&mutex);
    }

    /*We delete the queue and free the memory*/
    queue_destroy(q);
    pthread_exit((void *)(intptr_t)sum);
    return NULL;
}

/*-------------------------------MAIN FUNCTION-------------------------------*/
int main(int argc, const char *argv[])
{

    FILE *fp;

    /*We check the number of arguments*/
    if (argc != 5)
    {

        perror("Error: Invalid number of arguments");
        return -1;
    }

    /*Initilize threads*/
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&started, NULL);
    pthread_cond_init(&no_full, NULL);
    pthread_cond_init(&no_empty, NULL);

    /*We open the file and save the instructions*/
    fp = fopen(argv[1], "r");

    if (errno != 0)
    {

        perror("Error opening the file");
        return -1;
    }

    fscanf(fp, "%d", &numInstruc);
    if (numInstruc <= 0)
    {

        perror("Number of instructions wrong");
        return -1;
    }

    /*We save the file data in an array of elements and IDs*/
    int *id;
    struct element *elem;
    elem = malloc(sizeof(struct element) * numInstruc);
    id = malloc(sizeof(int) * numInstruc);
    int count = 0;
    int total_accum = 0;

    /*We read the type and time of the elements*/
    for (int j = 0; j < numInstruc; j++)
    {

        if (fscanf(fp, "%d %d %d", &id[j], &elem[j].type, &elem[j].time) != EOF)
        {
            count++;
        }
    }

    /*Close the file*/
    fclose(fp);
    if (errno != 0)
    {
        perror("Error closing the file");
        return -1;
    }

    /*If we are missing instruccions*/
    if (count < numInstruc)
    {
        perror("Instructions missing\n");
        return -1;
    }

    /*We save the arguments and check if they are OK*/
    int numProducers = atoi(argv[2]);
    int numConsumers = atoi(argv[3]);
    int BUFFER_SIZE = atoi(argv[4]);

    if (numProducers <= 0)
    {
        perror("Incorrect prod. number\n");
        return 1;
    }
    if (numConsumers <= 0)
    {
        perror("Incorrect cons. number\n");
        return 1;
    }

    if (BUFFER_SIZE <= 0)
    {
        perror("Incorrect buffer size\n");
        return 1;
    }

    /*Initilize circular queue*/
    q = queue_init(BUFFER_SIZE);

    /*---------------------Producers---------------------*/
    /*Dynamic vector that saves the producers.*/
    struct parameters *param = (struct parameters *)malloc(sizeof(struct parameters) * numProducers);
    param->elem = malloc(sizeof(struct element) * numInstruc);
    /*Distribute the number of instructions by the number of producers*/
    int distr = numInstruc / numProducers;
    int mod = numInstruc % numProducers;

    /*Creates the array of producers and consumers*/
    pthread_t ths[numProducers + numConsumers];

    /*Creates the producers*/
    for (int i = 0; i < numProducers; i++)
    {
        /*We set to the parameters structure every attribute and the initial and final instructions
        for every producer*/
        param[i].initial = i * distr;
        param[i].final = (i == numProducers - 1) ? (i + 1) * distr - 1 + mod : (i + 1) * distr - 1;
        param[i].elem = elem;
        if (pthread_create(&(ths[i]), NULL, producer, &param[i]) < 0)
        {
            perror("Error at creating producers threads");
        }

        pthread_mutex_lock(&mutex);
        while (has_started != 0)
        {
            pthread_cond_wait(&started, &mutex);
        }
        has_started = 0;
        pthread_mutex_unlock(&mutex);
    }

    /*---------------------Consumers---------------------*/

    /*Creates the consumers */
    for (int i = 0; i < numConsumers; i++)
    {

        if (pthread_create(&(ths[numProducers + i]), NULL, consumer, NULL) < 0)
        {
            perror("Error at creating consumers threads");
        }
        pthread_mutex_lock(&mutex);
        while (has_started != 0)
        {
            pthread_cond_wait(&started, &mutex);
        }
        has_started = 0;
        pthread_mutex_unlock(&mutex);
    }

    /*Wait for the producers to finish*/
    for (int i = 0; i < numProducers; i++)
    {
        pthread_join((ths[i]), NULL);
    }

    /*Wait for the consumers to finish*/
    for (int i = 0; i < numConsumers; i++)
    {
        pthread_join((ths[numProducers + i]), (void **)&total);
        total_accum = total_accum + total;
    }

    /*Once all the threads are done, we print the result*/
    printf("Total: %d euros.\n", total_accum);

    /*We free the memory and destroy the mutexs and the signals*/

    free(id);
    free(param);
    free(elem);
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&started);
    pthread_cond_destroy(&no_full);
    pthread_cond_destroy(&no_empty);

    return 0;
}