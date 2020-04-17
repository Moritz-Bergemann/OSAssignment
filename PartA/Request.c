#include "Request.h"
#include <stdlib.h>


/** Creates and initialises request buffer to default values
 */
RequestBuffer* createRequestBuffer(int size) 
{
    RequestBuffer* buffer;
    buffer = (RequestBuffer*)malloc(sizeof(RequestBuffer));
    
    buffer->reqArray = (Request**)malloc(sizeof(Request*) * size);
    buffer->size = size;
    buffer->used = 0;
    buffer->done = (int*)malloc(sizeof(int));
    *(buffer->done) = 0; //Initialising done flag to false
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

    //Freeing the buffer array & done flag
    free(buffer->reqArray);
    free(buffer->done);

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
    buffer->used++;

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

    pthread_mutex_lock(buffer->mutex); //Initiate lock on buffer

    //Check if buffer is empty and wait until it is not
    while (buffer->used <= 0)
    {
        //Wait for buffer producer to signal that new request has been added
        pthread_cond_wait(buffer->addedCond, buffer->mutex);
    }

    //Getting request from last slot in buffer array & decreasign used count
    request = buffer->reqArray[buffer->used - 1];
    buffer->used--;

    //Signal that a request has been removed from the buffer (i.e. buffer can no longer be full)
    pthread_cond_signal(buffer->removedCond);

    return request;
}

/** Continually checks buffer until it is empty and then marks 'done' flag as true
 *  (indicating that all requests have been fulfilled)
 */
void markDone(RequestBuffer* buffer)
{
    pthread_mutex_lock(buffer->mutex); //Locking buffer

    //Wait until all requests are removed from buffer
    while (buffer->used > 0) //While there are still requests in buffer
    {
        //Wait until a request is removed from the buffer
        pthread_cond_wait(buffer->removedCond, buffer->mutex);
    }

    //Set done to true as buffer is empty
    *(buffer->done) = 1;

    pthread_mutex_unlock(buffer->mutex); //Unlocking buffer
}