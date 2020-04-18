#ifndef LIFTR_H
#define LIFTR_H

#include "Request.h" //Needed as used in this file's structs/methods
#include <stdio.h>

//Structs//
//Information for the request thread
typedef struct {
    FILE* file; //File containing lift requests
    RequestBuffer* buffer; //Lift request buffer
} LiftRequestThreadInfo;

//Function headers//
void * request(void* liftReqInfoVoid);
Request* getRequest(FILE* file);
LiftRequestThreadInfo* createReqThreadInfo(RequestBuffer* buffer, FILE* reqFile);

#endif