#ifndef LIFTR_H
#define LIFTR_H

#include "Request.h" //Needed as used in this file's structs/methods
#include <stdio.h>
#include <semaphore.h>

//Structs//
//Information for the request process
typedef struct {
    FILE* reqFile; //File containing lift requests
    RequestBuffer* buffer; //Lift request buffer
    FILE* logFile;
    sem_t* logFileSem; //Binary semaphore for log file
    int requestNo; //Number of requests the process has received
} LiftRequestProcessInfo;

//Function headers//
int request(LiftRequestProcessInfo* info);
int getRequest(FILE* file, Request** requestAddr);
LiftRequestProcessInfo* createReqProcessInfo(RequestBuffer* buffer, FILE* reqFile, FILE* logFile, sem_t* logFileSem);
void logRequestReceived(FILE* logFile, sem_t* logFileSem, Request* request, int requestNo);

#endif