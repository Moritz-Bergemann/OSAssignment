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
    pthread_mutex_init(buffer->mutex, NULL); //Initialising mutex
    pthread_cond_init(buffer->emptyCond, NULL); //Initialising empty condition
    pthread_cond_init(buffer->fullCond, NULL); //Initialising full condition

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
        pthread_cond_wait(buffer->fullCond, buffer->mutex);
    }

    //Adding request to next open slot in buffer array & increasing used count
    buffer->reqArray[buffer->used] = request;
    buffer->used++;

    //Signal that a request has been added to the buffer (i.e. buffer can no longer be empty)
    pthread_cond_signal(buffer->emptyCond);

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
        pthread_cond_wait(buffer->emptyCond, buffer->mutex);
    }

    //Getting request from last slot in buffer array & decreasign used count
    request = buffer->reqArray[buffer->used - 1];
    buffer->used--;

    //Signal that a request has been removed from the buffer (i.e. buffer can no longer be full)
    pthread_cond_signal(buffer->fullCond);

    return request;
}