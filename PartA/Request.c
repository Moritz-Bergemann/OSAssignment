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
    
    buffer->reqQueue = (Request**)malloc(sizeof(Request*) * size);
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
        free((buffer->reqQueue)[ii]);
    }

    //Freeing the buffer array
    free(buffer->reqQueue);

    //Freeing mutexes & conditions
    free(buffer->mutex);
    free(buffer->addedCond);
    free(buffer->removedCond);

    //Freeing buffer itself
    free(buffer);
}

/** Adds the imported request to the imported buffer. Blocks if buffer is full until request can 
 *  successfully be added.
 */
void addRequestToBuffer(Request* request, RequestBuffer* buffer)
{
    printf("Buffer: ADD WAITING FOR INTIAL LOCK\n"); //DEBUG
    pthread_mutex_lock(buffer->mutex); //Initiate lock on buffer
    printf("Buffer: LOCKED FOR REQUEST ADD\n"); //DEBUG
    
    //Check if the buffer is full and wait until it is not
    while (buffer->used >= buffer->size) /*While is necessary as other stuff may have refilled buffer 
        after signalling before this process gets its turn*/
    {
        //Wait for one of lift (consumer) processes to signal they have removed a request from the buffer
        printf("Buffer: FULL (used=%d), WAITING FOR REMOVALS\n", buffer->used); //DEBUG
        pthread_cond_wait(buffer->removedCond, buffer->mutex);
        printf("Buffer: REMOVAL WAIT COMPLETE\n"); //DEBUG

    }

    //Adding request to next open slot in buffer array & increasing used count
    buffer->reqQueue[buffer->used] = request;
    (buffer->used)++;

    printf("Buffer: Request %d-%d added\n", request->start, request->dest); //DEBUG

    //Signal that a request has been added to the buffer (i.e. buffer can no longer be empty)
    pthread_cond_signal(buffer->addedCond);

    pthread_mutex_unlock(buffer->mutex); //Release lock on buffer
    printf("Buffer: UNLOCKED AFTER REQUEST ADD\n"); //DEBUG
}

/** Returns a request from front the imported buffer. Blocks if buffer is empty until request can successfully
 *  be removed, or until timeout occurs.
 *  Timeout (where no requests added over a given time period) included so that if this method is called once/during
 *      the removal of all requests (& the finishing of the file so no more are added) the wait will not be performed
 *      indefinitely, allowing the thread to return to main and exit gracefully.
 */
Request* getRequestFromBuffer(RequestBuffer* buffer)
{
    Request* request;
    int waitStatus = 0;
    struct timespec* specTime = (struct timespec*)malloc(sizeof(struct timespec));

    //printf("Buffer: RETRIEVAL TIMEOUT TIME - %d seconds, %d nanoseconds\n", BUFFER_TIMEOUT_S, BUFFER_TIMEOUT_NS); //DEBUG

    printf("Buffer: RETRIEVAL WAITING ON INTIIAL LOCK\n"); //DEBUG
    pthread_mutex_lock(buffer->mutex); //Initiate lock on buffer
    printf("Buffer: LOCKED FOR RETRIEVAL\n"); //DEBUG

    //Check if buffer is empty and wait until it is not (or until timeout occurs)
    while (buffer->used <= 0 && waitStatus == 0)
    {
        //Setting up timespec
        specTime->tv_sec = time(NULL) + BUFFER_TIMEOUT_S; //Adds seconds waiting time
        specTime->tv_nsec = BUFFER_TIMEOUT_NS; //Adds nanoseconds waiting time
        //Wait for buffer producer to signal that new request has been added (releases lock)
        printf("Buffer: EMPTY (used=%d), WAITING FOR NEW RESOURCES\n", buffer->used); //DEBUG
        printf("\t Waiting for: %lld\n", (long long) specTime->tv_sec); //DEBUG
        waitStatus = pthread_cond_timedwait(buffer->addedCond, buffer->mutex, specTime);
        printf("Buffer: NEW RESOURCE WAIT COMPLETE\n");

        if (waitStatus != 0)
        {
            if (waitStatus == ETIMEDOUT)
            {
                printf("Buffer: WAIT TIMED OUT (Error code %d)\n", waitStatus); //DEBUG
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
        //Getting request from front slot in buffer array 
        request = buffer->reqQueue[0];

        //Shuffling all other entries towards front
        int ii;
        for (ii = 1; ii < buffer->used; ii++)
        {
            buffer->reqQueue[ii - 1] = buffer->reqQueue[ii];
        }

        (buffer->used)--;

        //Signal that a request has been removed from the buffer (i.e. buffer can no longer be full)
        pthread_cond_signal(buffer->removedCond);

        printf("Buffer: Request %d-%d retrieved\n", request->start, request->dest); //DEBUG
    }

    free(specTime);

    pthread_mutex_unlock(buffer->mutex); //Release lock on buffer
    printf("Buffer: LOCK RELEASED AFTER RETRIEVAL\n");

    return request;
}

/** Continually checks buffer until it is empty and then marks 'done' flag as true
 *  (indicating that all requests have been fulfilled)
 */
void markDone(RequestBuffer* buffer)
{
    printf("Buffer: Waiting for empty to mark as done\n"); //DEBUG
    printf("Buffer: Initial used - %d\n", buffer->used); //DEBUG

    printf("Buffer: MARKDONE WAITING ON INTIIAL LOCK\n"); //DEBUG
    pthread_mutex_lock(buffer->mutex); //Locking buffer
    printf("Buffer: LOCKED FOR MARKDONE CHECK\n"); //DEBUG

    //Wait until all requests are removed from buffer
    while (buffer->used > 0) //While there are still requests in buffer
    {
        printf("Buffer: Waiting on empty to mark done (current - %d)\n", buffer->used); //DEBUG
        //Wait until a request is removed from the buffer
        printf("Buffer: STARTING MARKDONE WAIT\n"); //DEBUG
        pthread_cond_wait(buffer->removedCond, buffer->mutex);
        printf("Buffer: MARKDONE WAIT COMPLETE\n"); //DEBUG
    }

    //Set done to true as buffer is empty
    buffer->done = 1;

    pthread_mutex_unlock(buffer->mutex); //Unlocking buffer
    printf("Buffer: UNLOCKED AFTERFOR MARKDONE CHECK\n"); //DEBUG
}