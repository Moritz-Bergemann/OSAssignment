/**LiftR by Moritz Bergemann
 * File containing functionality for reading lift requests from the imported file writing these to the request buffer.
 */
#include "LiftR.h"
#include "Request.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>


void *request(void* liftReqInfoVoid)
{
    printf("Hi, I'm the elevator request handler\n");

    pthread_exit(NULL);
}

/** Creates and initialises LiftRequestThreadInfo struct
 */
LiftRequestThreadInfo* createReqThreadInfo(RequestBuffer* buffer, FILE* reqFile)
{
    //Creating request info on heap
    LiftRequestThreadInfo* info;
    info = (LiftRequestThreadInfo*)malloc(sizeof(LiftRequestThreadInfo));

    //Initialising struct values
    info->buffer = buffer;
    info-> reqFile = reqFile;

    return info;
}