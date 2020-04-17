#ifndef LIFT_H
#define LIFT_H

#include "Request.h" //Needed as used in this file's structs/methods

//Structs//
//Information for the lift process
typedef struct {
    int liftNum; //Identifying number of this lift (probably 1-3)
    int totalMovement; //Total movement (number of floors) done by this lift
    RequestBuffer* buffer; //Lift request buffer
    int* done; //Tracks whether all requests have been read from lift request file
} LiftThreadInfo;

//Information for lift operation (movement from one floor to another)
typedef struct {
    int startFloor;
    int endFloor;
} Operation;

typedef struct {
    int operationNum; //Number of operation for this lift
    int prevPos;
    int FinalPos; 
    int movement; //Amount of movement performed for this request
    Request* request; //Request being fulfilled

    //Lift has at most 2 operations (only 1 if already on request floor) 2nd should be NULL if not performed
    Operation* op1;
    Operation* op2;
} LiftOperation;

//Function headers//
void *lift(void* liftInfoVoid);
LiftThreadInfo* createLiftThreadInfo(RequestBuffer* buffer, int liftNum);

#endif