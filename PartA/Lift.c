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

    Request* curRequest; //Current request being worked on
    LiftOperation* curOperation;

    //Attempt to read requests while all requests from file have not been completed
    while (!info->buffer->done) //If timeout occurs, this check will continue to be performed FIXME DOES THIS NEED TO GET A LOCK ON BUFFER
    {
        curRequest = getRequestFromBuffer(info->buffer);

        if (curRequest != NULL) //If getting request did not time out
        {
            curOperation = performOperation(curRequest);
            
            info->totalMovement += curOperation->movement;

            logOperation(curOperation);

            //Freeing components of current operation (including its associated request)
            freeOperation(curOperation);
        }
        else
        {
            printf("Lift-%d: Request retrieval timed out\n"); //DEBUG
        }
    }
    

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
    info->totalMovement = 0;

    return info;
}

/** Frees the components of the imported LiftOperation AND its associated request
 */
void freeOperation(LiftOperation* op)
{
    free(op->op1);
    if (op->op2 != NULL) //If second operation exists
    {
        free(op->op2);
    }

    free(op->request);

    free(op);
}

//TODO performOperation

//TODO logOperation