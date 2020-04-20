#include "Lift.h"
#include "Main.h"
#include "Request.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

void *lift(void* infoVoid)
{
    // printf("Hi, I'm lift process number %d\n", liftInfo->liftNum); //DEBUG

    //Getting lift information from lift
    LiftProcessInfo* info;
    info = (LiftProcessInfo*)infoVoid;

    Request* curRequest; //Current request being worked on
    LiftOperation* curOperation;

    //Attempt to read requests while all requests from file have not been completed
    printf("Lift-%d: Starting retrievals\n", info->liftNum); //DEBUG
    while (!(info->buffer->done)) //If timeout occurs, this check will continue to be performed FIXME DOES THIS NEED TO GET A LOCK ON BUFFER
    {
        printf("Lift-%d: Trying to get request from buffer\n", info->liftNum); //DEBUG
        curRequest = getRequestFromBuffer(info->buffer);

        if (curRequest != NULL) //If getting request did not time out
        {
            printf("Lift-%d: Performing Request %d to %d...\n", info->liftNum, curRequest->start, curRequest->dest); //DEBUG
            curOperation = performOperation(curRequest, info->curPosition, info->moveTime);
            info->totalMovement += curOperation->movement;

            printf("Lift-%d: Request complete!\n", info->liftNum); //DEBUG

            logLiftOperation(info->logFile, info->logFileMutex, info->liftNum, 
                                info->operationNo, curOperation, info->totalMovement);

            (info->operationNo)++;

            //Freeing components of current operation (including its associated request)
            freeLiftOperation(curOperation);
        }
        else
        {
            printf("Lift-%d: Request retrieval timed out\n", info->liftNum); //DEBUG
        }
    }
    
    printf("Lift-%d: operation complete! Terminating...\n", info->liftNum);

    pthread_exit(NULL);
}

/** Creates and initialises LiftProcessInfo struct
 */
LiftProcessInfo* createLiftProcessInfo(RequestBuffer* buffer, int liftNum, int moveTime, FILE* logFile, pthread_mutex_t* logFileMutex)
{
    //Creating request info on heap
    LiftProcessInfo* info;
    info = (LiftProcessInfo*)malloc(sizeof(LiftProcessInfo));

    //Initialising struct values
    info->buffer = buffer;
    info->liftNum = liftNum;
    info->moveTime = moveTime;
    info->totalMovement = 0;
    info->operationNo = 1;
    info->logFile = logFile;
    info->logFileMutex = logFileMutex;
    info->curPosition = (int*)malloc(sizeof(int));
    *(info->curPosition) = 1;

    return info;
}

/** Frees the LiftRequestProcessInfo struct at the imported pointer and its contents.
 */
void freeLiftProcessInfo(LiftProcessInfo* info)
{
    free(info->curPosition);
    free(info);
}

/** Frees the components of the imported LiftOperation AND its associated request
 */
void freeLiftOperation(LiftOperation* op)
{
    free(op->move1);
    if (op->move2 != NULL) //If second operation exists
    {
        free(op->move2);
    }

    free(op->request);

    free(op);
}

/** Performs lift operation for imported lift request
 * 
 */
LiftOperation* performOperation(Request* request, int* curPosition, int moveTime)
{
    //Initialising lift operation
    LiftOperation* operation;
    operation = (LiftOperation*)malloc(sizeof(LiftOperation));
    operation->request = request;
    operation->prevPos = *curPosition;
    operation->movement = 0;
    operation->move1 = NULL;
    operation->move2 = NULL;
    
    //Performing movement to request floor (if not already there)
    if (*curPosition != request->start)
    {
        //Performing 1st lift move (to request start)
        operation->move1 = liftMove(*curPosition, request->start, moveTime);
        operation->movement += operation->move1->movement;
        *curPosition = operation->move1->endFloor;

        //Performing 2nd lift move (to request destination)
        operation->move2 = liftMove(*curPosition, request->dest, moveTime);
        operation->movement += operation->move2->movement;
        *curPosition = operation->move2->endFloor;
    }
    else //Performing movement if already at request start
    {
        //Performing only lift move (to request destination)
        operation->move1 = liftMove(*curPosition, request->dest, moveTime);
        operation->movement += operation->move1->movement;
        *curPosition = operation->move1->endFloor;
    }
    
    operation->finalPos = *curPosition;
    
    return operation;
} 

LiftMovement* liftMove(int start, int end, int moveTime)
{
    LiftMovement* move = (LiftMovement*)malloc(sizeof(LiftMovement));
    
    //Set starting floor
    move->startFloor = start;
    
    //Wait specified amount of time for lift to move
    sleep(moveTime);

    //Set ending floor
    move->endFloor = end;

    //Set amount moved (between floors)
    move->movement = abs(end - start);

    return move;
}

void logLiftOperation(FILE* file, pthread_mutex_t* fileMutex, int liftNum, int operationNo, LiftOperation* op, int totalMovement)
{
    printf("Lift: Making log...\n");
    pthread_mutex_lock(fileMutex); //Locking log file

    fprintf(file, "Lift-%d Operation\n", liftNum);
    fprintf(file, "Previous position: Floor %d\n", op->prevPos);
    fprintf(file, "Request: Floor %d to Floor %d\n", op->request->start, op->request->dest);
    fprintf(file, "Detail operations:\n");
    fprintf(file, "\tGo from Floor %d to Floor %d\n", op->move1->startFloor, op->move1->endFloor);
    if (op->move2 != NULL) //If second move was performed
    {
        fprintf(file, "\tGo from Floor %d to Floor %d\n", op->move2->startFloor, op->move2->endFloor);
    }
    fprintf(file, "\t#movement for this request: %d\n", op->movement);
    fprintf(file, "\t#request: %d\n", operationNo);
    fprintf(file, "\tTotal #movement: %d\n", totalMovement);
    fprintf(file, "Current position: floor %d\n", op->finalPos);
    fprintf(file, "\n");

    pthread_mutex_unlock(fileMutex); //Release lock on log file
}