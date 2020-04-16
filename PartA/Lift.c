#include <stdio.h>
#include <pthread.h>

#include "Lift.h"
#include "Main.h"

void *lift(void* liftInfoVoid)
{
    //Getting lift information from lift
    LiftThreadInfo* liftInfo;
    liftInfo = (LiftThreadInfo*)liftInfoVoid;

    printf("Hi, I'm lift process number %d\n", liftInfo->liftNum);

    pthread_exit(NULL);
}