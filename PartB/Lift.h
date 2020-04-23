#ifndef LIFT_H
#define LIFT_H

#include "Request.h" //Needed as used in this file's structs/methods
#include <stdio.h>
#include <semaphore.h>

//Structs//
//Information for the lift process
typedef struct {
    int liftNum; //Identifying number of this lift (probably 1-3)
    int moveTime; //Time taken to move from one floor to another
    int totalMovement; //Total movement (number of floors) done by this lift
    int* curPosition; //Current position of the lift (pointer so can be externally modified)
    RequestBuffer* buffer; //Lift request buffer
    int operationNo; //Number of requests this lift has fulfilled
    char* logFilePath; //Path to log file
    sem_t* logFileSem; //Binary semaphore for log file
    int* sharedTotalMovement; 
} LiftProcessInfo;

//Information for lift movement (from one floor to another)
typedef struct {
    int startFloor;
    int endFloor;
    int movement; //Amount of floors moved in movement
} LiftMovement;

//Information for a lift operation (fulfilment of a request)
typedef struct {
    int prevPos;
    int finalPos; 
    int movement; //Amount of movement performed for this request
    Request* request; //Request being fulfilled

    //Lift has at most 2 operations (only 1 if already on request floor) 2nd should be NULL if not performed
    LiftMovement* move1;
    LiftMovement* move2;
} LiftOperation;

//Function headers//
int lift(LiftProcessInfo* info);
LiftProcessInfo* createLiftProcessInfo(RequestBuffer* buffer, int liftNum, int moveTime, char* logFilePath, sem_t* logFileSem, int* sharedTotalMovement);
void freeLiftProcessInfo(LiftProcessInfo* info);
void freeLiftOperation(LiftOperation* op);
LiftOperation* performOperation(Request* request, int* curPosition, int moveTime);
LiftMovement* liftMove(int start, int end, int moveTime);
void logLiftOperation(char* logFilePath, sem_t* logFileSem, int liftNum, int operationNo, LiftOperation* op, int totalMovement);

#endif