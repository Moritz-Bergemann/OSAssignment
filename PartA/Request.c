#include "Request.h"
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <errno.h>

/** Creates and initialises request buffer to default values
 */
RequestBuffer* createRequestBuffer(int size) 
{
    RequestBuffer* buffer;
    buffer = (RequestBuffer*)malloc(sizeof(RequestBuffer));
    
    buffer->reqArray = (Request**)malloc(sizeof(Request*) * size);
    buffer->size = size;
    buffer->used = 0;
    buffer->done = 0; //Initialising done flag to false

    buffer->mutex = (pthread_mutex_t*)malloc(sizeof(pthread_mutex_t));
    buffer->addedCond = (pthread_cond_t*)malloc(sizeof(pthread_cond_t));
    buffer->removedCond = (pthread_cond_t*)malloc(sizeof(pthread_cond_t));
    pthread_mutex_init(buffer->mutex, NULL); //Initialising mutex
    pthread_cond_init(buffer->addedCond, NULL); //Initialising empty condition
    pthread_cond_init(buffer->removedCond, NULL); //Initialising full condition

    return buffer;
}

/** Free the imported buffer and all its contents
 */
void freeRequestBuffer(RequestBuffer* buffer)
{
    //Freeing each existing request struct in buffer
    int ii;
    for (ii = 0; ii < buffer->used; ii++)
    {
        free((buffer->reqArray)[ii]);
    }

    //Freeing the buffer array
    free(buffer->reqArray);

    //Freeing buffer itself
    free(buffer);
}

/** Adds the imported request to the imported buffer. Blocks if buffer is full until request can 
 *  successfully be added.
 */
void addRequestToBuffer(Request* request, RequestBuffer* buffer)
{
    pthread_mutex_lock(buffer->mutex); //Initiate lock on buffer
    
    //Check if the buffer is full and wait until it is not  //TODO CHECK IF THIS IS RIGHT
    while (buffer->used >= buffer->size) /*While is necessary as other stuff may have refilled buffer 
        after signalling before this process gets its turn*/
    {
        //Wait for one of lift (consumer) processes to signal they have removed a request from the buffer
        pthread_cond_wait(buffer->removedCond, buffer->mutex);
    }

    //Adding request to next open slot in buffer array & increasing used count
    buffer->reqArray[buffer->used] = request;
    (buffer->used)++;

    printf("Buffer: Request %d-%d added\n", request->start, request->dest); //DEBUG

    //Signal that a request has been added to the buffer (i.e. buffer can no longer be empty)
    pthread_cond_signal(buffer->addedCond);

    pthread_mutex_unlock(buffer->mutex); //Release lock on buffer
}

/** Returns a request from the imported buffer. Blocks if buffer is empty until request can successfully
 *  be removed
 */
Request* getRequestFromBuffer(RequestBuffer* buffer)
{
    Request* request;
    int waitStatus = 0;
    struct timespec* specTime = (struct timespec*)malloc(sizeof(struct timespec));

    pthread_mutex_lock(buffer->mutex); //Initiate lock on buffer

    //Check if buffer is empty and wait until it is not (or until timeout occurs)
    while (buffer->used <= 0 && waitStatus == 0)
    {
        //Setting up timespec
        localtime(&(specTime->tv_sec)); //First gets current epoch time
        specTime->tv_sec += BUFFER_TIMEOUT_S; //Adds seconds waiting time
        specTime->tv_nsec = BUFFER_TIMEOUT_NS; //Adds nanoseconds waiting time

        //Wait for buffer producer to signal that new request has been added
        waitStatus = pthread_cond_timedwait(buffer->addedCond, buffer->mutex, specTime);

        if (waitStatus != 0)
        {
            if (waitStatus == ETIMEDOUT)
            {
                printf("Buffer: request timed out\n"); //DEBUG
            }
            else //If other error occured
            {
                printf("Buffer: Request retrival wait failed with non-standard error\n"); //DEBUG
            }

            //Setting request to null to return (indicating timeout/error)
            request = NULL;
        }
    }

    if (waitStatus == 0) //If a request is available to be retrieved (no timeout)
    {
        //Getting request from last slot in buffer array & decreasing used count
        request = buffer->reqArray[buffer->used - 1];
        (buffer->used)--;

        //Signal that a request has been removed from the buffer (i.e. buffer can no longer be full)
        pthread_cond_signal(buffer->removedCond);

        printf("Buffer: Request %d-%d retrieved\n", request->start, request->dest); //DEBUG
    }

    pthread_mutex_unlock(buffer->mutex); //Release lock on buffer

    return request;
}

/** Continually checks buffer until it is empty and then marks 'done' flag as true
 *  (indicating that all requests have been fulfilled)
 */
void markDone(RequestBuffer* buffer)
{
    printf("Buffer: Waiting for empty to mark as done\n"); //DEBUG
    printf("Buffer: Initial used - %d\n", buffer->used); //DEBUG

    pthread_mutex_lock(buffer->mutex); //Locking buffer

    //Wait until all requests are removed from buffer
    while (buffer->used > 0) //While there are still requests in buffer
    {
        printf("Buffer: Waiting on empty to mark done (current - %d)\n", buffer->used); //DEBUG
        //Wait until a request is removed from the buffer
        pthread_cond_wait(buffer->removedCond, buffer->mutex);
    }

    //Set done to true as buffer is empty
    buffer->done = 1;

    pthread_mutex_unlock(buffer->mutex); //Unlocking buffer
}