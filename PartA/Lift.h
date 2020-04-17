#ifndef LIFT_H
#define LIFT_H

#include "Request.h" //Needed as used in this file's structs/methods

//Structs//
//Information for the lift process
typedef struct {
    int liftNum; //Identifying number of this lift (probably 1-3)
    RequestBuffer* buffer; //Lift request buffer
} LiftThreadInfo;

//Function headers//
void *lift(void* liftInfoVoid);
LiftThreadInfo* createLiftThreadInfo(RequestBuffer* buffer, int liftNum);

#endif