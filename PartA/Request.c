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

    return buffer;
}

void freeRequestBuffer(RequestBuffer* buffer)
{
    //Freeing each existing request struct in buffer
    int ii;
    for (ii = 0; ii < buffer->used; ii++)
    {
        free((buffer->reqArray)[ii]);
    }

    //Freeing buffer itself
    free(buffer);
}