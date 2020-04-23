#ifndef LIFTR_H
#define LIFTR_H

#include "Request.h" //Needed as used in this file's structs/methods
#include <stdio.h>
#include <semaphore.h>

//Structs//
//Information for the request process
typedef struct {
    char* reqFilePath; //Path to file containing lift requests
    RequestBuffer* buffer; //Lift request buffer
    char* logFilePath; //Path to log file
    sem_t* logFileSem; //Binary semaphore for log file
    int requestNo; //Number of requests the process has received
    int* sharedTotalRequests; //Number of total requests shared with main process
} LiftRequestProcessInfo;

//Function headers//
int request(LiftRequestProcessInfo* info);
int getRequest(FILE* file, Request** requestAddr);
LiftRequestProcessInfo* createReqProcessInfo(RequestBuffer* buffer, char* reqFilePath, char* logFilePath, sem_t* logFileSem, int* sharedTotalRequests);
void logRequestReceived(char* logFilePath, sem_t* logFileSem, Request* request, int requestNo);

#endif