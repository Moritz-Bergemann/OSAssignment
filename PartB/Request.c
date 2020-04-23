#define _GNU_SOURCE 
#include "Request.h"
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <errno.h>
#include <sys/mman.h>


/** Creates and initialises request buffer in shared memory to default values
 */
RequestBuffer* createRequestBuffer(int size) 
{    
    RequestBuffer* buffer = (RequestBuffer*)createSharedMemory(sizeof(RequestBuffer));

    buffer->reqQueue = (Request*)createSharedMemory(sizeof(Request) * size);
    buffer->size = size;
    buffer->used = 0;
    buffer->done = 0; //Initialising done flag to false

    buffer->sem = (sem_t*)createSharedMemory(sizeof(sem_t));
    buffer->fullSem = (sem_t*)createSharedMemory(sizeof(sem_t));
    buffer->emptySem = (sem_t*)createSharedMemory(sizeof(sem_t));
    sem_init(buffer->sem, 1, 1); //Initialising main buffer semaphore
    sem_init(buffer->fullSem, 1, 0); //Initialising full semaphore
    sem_init(buffer->emptySem, 1, 20); //Initialising empty semaphore (size initially since all slots 'empty')

    return buffer;
}

void* createSharedMemory(int size)
{
    //Setting memory to be readable & writeable
    int protection = PROT_READ | PROT_WRITE;

    int visibility = MAP_SHARED | MAP_ANONYMOUS;

    //Creating & returning shared memory
    return mmap(NULL, size, protection, visibility, -1, 0); //TODO check last 2 params
}


/** Free the imported buffer and all its contents
 */
void freeRequestBuffer(RequestBuffer* buffer)
{
    //Freeing the buffer array
    munmap(buffer->reqQueue, sizeof(Request) * buffer->size);

    //Freeing semaphores    
    sem_destroy(buffer->sem);
    sem_destroy(buffer->fullSem);
    sem_destroy(buffer->emptySem);    
    munmap(buffer->sem, sizeof(sem_t));
    munmap(buffer->fullSem, sizeof(sem_t));
    munmap(buffer->emptySem, sizeof(sem_t));

    //Freeing buffer itself
    munmap(buffer, sizeof(RequestBuffer));
}

/** Adds the imported request to the imported buffer. Blocks if buffer is full until request can 
 *  successfully be added.
 */
void addRequestToBuffer(Request* request, RequestBuffer* buffer)
{
    //Indicating item being added (but NOT that item has been added) AND waiting for buffer to no longer be full
    sem_wait(buffer->emptySem); 

    printf("Buffer: ADD WAITING FOR INTIAL LOCK\n"); //DEBUG
    sem_wait(buffer->sem); //Wait for/initiate lock on buffer
    printf("Buffer: LOCKED FOR REQUEST ADD\n"); //DEBUG

    //Adding request to next open slot in buffer array & increasing used count
    (buffer->reqQueue[buffer->used]).start = request->start;
    (buffer->reqQueue[buffer->used]).dest = request->dest;
    (buffer->used)++;

    printf("Buffer: Request %d-%d added\n", request->start, request->dest); //DEBUG
    
    //Releasing lock on buffer
    sem_post(buffer->sem);
    printf("Buffer: UNLOCKED AFTER REQUEST ADD\n"); //DEBUG

    //Signal that a request has been added to the buffer (i.e. buffer can no longer be empty)
    sem_post(buffer->fullSem);
}

/** Returns a request from front the imported buffer (as a copy). Blocks if buffer is empty until request can successfully
 *  be removed, or until timeout occurs.
 *  Timeout (where no requests added over a given time period) included so that if this method is called once/during
 *      the removal of all requests (& the finishing of the file so no more are added) the wait will not be performed
 *      indefinitely, allowing the process to return to main and exit gracefully.
 */
Request* getRequestFromBuffer(RequestBuffer* buffer)
{
    Request* request;
    int waitStatus = 0;
    struct timespec* specTime = (struct timespec*)malloc(sizeof(struct timespec));

    //printf("Buffer: RETRIEVAL TIMEOUT TIME - %d seconds, %d nanoseconds\n", BUFFER_TIMEOUT_S, BUFFER_TIMEOUT_NS); //DEBUG
    //Setting up timespec
    specTime->tv_sec = time(NULL) + BUFFER_TIMEOUT_S; //Adds seconds waiting time
    specTime->tv_nsec = BUFFER_TIMEOUT_NS; //Adds nanoseconds waiting time

    //Indicating request to be removed from buffer AND waiting in case item is added to buffer
    printf("Buffer: RETRIEVAL CHECKING/WAITING FOR NON-EMPTY BUFFER\n");
    // printf("\t Waiting for: %lld\n", (long long) specTime->tv_sec); //DEBUG
    waitStatus = sem_timedwait(buffer->fullSem, specTime);

    if (waitStatus != 0) //If wait did not allow for semaphore retrieval
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
    else //If wait allowed for semaphore retrieval
    {
        //Getting lock on buffer
        printf("Buffer: RETRIEVAL WAITING ON BUFFER LOCK\n"); //DEBUG
        sem_wait(buffer->sem);
        printf("Buffer: LOCKED FOR RETRIEVAL\n"); //DEBUG

        //Creating copy of request in front slot of buffer array 
        request = (Request*)malloc(sizeof(Request));
        request->start = buffer->reqQueue[0].start;
        request->dest = buffer->reqQueue[0].dest;

        //Shuffling all other entries towards front
        for (int ii = 1; ii < buffer->used; ii++)
        {
            buffer->reqQueue[ii - 1] = buffer->reqQueue[ii]; //FIXME make sure this works
        }

        (buffer->used)--;

        sem_post(buffer->sem); //Release lock on buffer
        printf("Buffer: LOCK RELEASED AFTER RETRIEVAL\n");

        //Signal that a request has been removed from the buffer (i.e. buffer can no longer be full)
        sem_post(buffer->emptySem);
        printf("Buffer: Request %d-%d retrieved\n", request->start, request->dest); //DEBUG
    }

    free(specTime);

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
    sem_wait(buffer->sem);
    printf("Buffer: LOCKED FOR MARKDONE CHECK\n"); //DEBUG

    while(buffer->used > 0)
    {
        printf("Buffer: Waiting on empty to mark done (current - %d)\n", buffer->used); //DEBUG

        //Unlock buffer
        sem_post(buffer->sem);

        //Wait to access lock again to see if requests have been removed
        printf("Buffer: STARTING MARKDONE WAIT\n"); //DEBUG
        sem_wait(buffer->sem);
        printf("Buffer: MARKDONE WAIT COMPLETE\n"); //DEBUG
    }

    //Set done to true as buffer is empty
    buffer->done = 1;

    sem_post(buffer->sem); //Unlocking buffer
    printf("Buffer: UNLOCKED AFTERFOR MARKDONE CHECK\n"); //DEBUG
}