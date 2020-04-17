#include "Lift.h"
#include "Main.h"
#include "Request.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>


void *lift(void* liftInfoVoid)
{
    //Getting lift information from lift
    LiftThreadInfo* liftInfo;
    liftInfo = (LiftThreadInfo*)liftInfoVoid;

    printf("Hi, I'm lift process number %d\n", liftInfo->liftNum);

    pthread_exit(NULL);
}

/** Creates and initialises LiftThreadInfo struct
 */
LiftThreadInfo* createLiftThreadInfo(RequestBuffer* buffer, int liftNum)
{
    //Creating request info on heap
    LiftThreadInfo* info;
    info = (LiftThreadInfo*)malloc(sizeof(LiftThreadInfo));

    //Initialising struct values
    info->buffer = buffer;
    info->liftNum = liftNum;

    return info;
}
