#include "Lift.h"
#include "Main.h"
#include "Request.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>


void *lift(void* infoVoid)
{
    // printf("Hi, I'm lift process number %d\n", liftInfo->liftNum); //DEBUG

    //Getting lift information from lift
    LiftThreadInfo* info;
    info = (LiftThreadInfo*)infoVoid;

    //Attempt to read requests while all requests from file have not been completed
    while (!info->done)
    {
        
    }
    

    pthread_exit(NULL);
}

/** Creates and initialises LiftThreadInfo struct
 */
LiftThreadInfo* createLiftThreadInfo(RequestBuffer* buffer, int liftNum, int* doneTracker)
{
    //Creating request info on heap
    LiftThreadInfo* info;
    info = (LiftThreadInfo*)malloc(sizeof(LiftThreadInfo));

    //Initialising struct values
    info->buffer = buffer;
    info->liftNum = liftNum;
    info->totalMovement = 0;
    info->done = doneTracker;

    return info;
}
