#ifndef LIFTR_H
#define LIFTR_H

#include "Request.h" //Needed as used in this file's structs/methods
#include <stdio.h>

//Structs//
//Information for the request thread
typedef struct {
    FILE* reqFile; //File containing lift requests
    RequestBuffer* buffer; //Lift request buffer
    FILE* logFile;
    pthread_mutex_t* logFileMutex; //Mutex for log file
    int requestNo; //Number of requests the thread has received
} LiftRequestThreadInfo;

//Function headers//
void * request(void* liftReqInfoVoid);
Request* getRequest(FILE* file);
LiftRequestThreadInfo* createReqThreadInfo(RequestBuffer* buffer, FILE* reqFile, FILE* logFile, pthread_mutex_t* logFileMutex);

#endif