/**LiftR by Moritz Bergemann
 * File containing functionality for reading lift requests from the imported file writing these to the request buffer.
 */
#include "LiftR.h"
#include "Request.h"
#include "Main.h"
#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>

/** Primary method of LiftR (lift request handler) process.
 *  Performs file reading, validating & adding contents to the array.
 */
int request(LiftRequestProcessInfo* info)
{
    // printf("Hi, I'm the elevator request handler\n"); //DEBUG

    Request* newRequest = NULL;
    int fileEnded = 0; //Tracks whether requests have been exhausted (default false)
    int lineNum = 0; //Tracks the current line number (for error messages)

    FILE* reqFile = fopen(info->reqFilePath, "r");
    
    if (reqFile != NULL) //If request file successfully opened
    {
        while (!fileEnded)
        {
            lineNum++;

            //Getting request from file (placed into pointer, returns whether EOF reached)
            fileEnded = getRequest(reqFile, &newRequest);
            
            if (newRequest != NULL) //If request was successfully read from line in file
            {
                addRequestToBuffer(newRequest, info->buffer);         
                
                info->requestNo++;

                logRequestReceived(info->logFilePath, info->logFileSem, newRequest, info->requestNo);

                printf("LiftR: Finished logging request %d-%d\n", newRequest->start, newRequest->dest); //DEBUG

                //Freeing request (since buffer created a copy in shared memory)
                free(newRequest);

                printf("LiftR: Finished free\n"); //TEMP DEBUG
            }
            else //If request was invalid
            {
                printf("LiftR: Invalid line in request file - line %d\n", lineNum); //DEBUG
            }
        }

        //Marking all requests as complete once buffer is empty
        markDone(info->buffer);

        printf("LiftR: All requests read from file!\n"); //DEBUG
        printf("LiftR: Exiting...\n"); //DEBUG
    }
    else
    {
        printf("LiftR: Failed to open request file! Aborting operation...\n");
        
        //Marking buffer operation as 'done' to inform lift processes they should exit
        info->buffer->done = 1;
    }
    
    //Setting number of total requests to be shared with main process
    *(info->sharedTotalRequests) = info->requestNo;

    return 0;
}

/** Reads a line from the imported file & attempts to parse it as a lift request.
 *  Lift request pointer is made NULL if invalid or file ended
 *  Returns 1 if file has ended & 0 if not
 */
int getRequest(FILE* file, Request** requestAddr)
{
    Request* newRequest = NULL;

    int sSuccesses;
    int startFloor, destFloor;

    int fileEnded = 0;

    sSuccesses = fscanf(file, "%d %d", &startFloor, &destFloor);

    if (sSuccesses == 2)
    {
        printf("LiftR: Line scanned successfully\n"); //DEBUG
        
        if ((startFloor >= 1) && (startFloor <= NUM_FLOORS)) //Validating starting floor range
        {
            if ((destFloor >= 1) && (destFloor <= NUM_FLOORS)) //Validating destination floor range
            {
                // printf("LiftR: Line is valid!\n"); //DEBUG
                
                //Creating request struct & giving it the validated values
                newRequest = (Request*)malloc(sizeof(Request));
                newRequest->start = startFloor;
                newRequest->dest = destFloor;
            }
            else
            {
                printf("LiftR: Destination floor not in valid range\n");
            }
            
        }
        else
        {
            printf("LiftR: Starting floor not in valid range\n");
        }
    }
    else if (sSuccesses == EOF)
    {
        printf("LiftR: Reached end of file!\n"); //Debug
        fileEnded = 1;
    }
    else
    {
        printf("LiftR: Failed to read line!\n"); //Debug
    }
    
    *requestAddr = newRequest;
    return fileEnded;
}

/** Creates and initialises LiftRequestProcessInfo struct
 */
LiftRequestProcessInfo* createReqProcessInfo(RequestBuffer* buffer, char* reqFilePath, char* logFilePath, sem_t* logFileSem, int* sharedTotalRequests)
{
    //Creating request info on heap
    LiftRequestProcessInfo* info;
    info = (LiftRequestProcessInfo*)malloc(sizeof(LiftRequestProcessInfo));

    //Initialising struct values
    info->buffer = buffer;
    info->reqFilePath = reqFilePath;
    info->requestNo = 0;
    info->logFilePath = logFilePath;
    info->logFileSem = logFileSem;

    info->sharedTotalRequests = sharedTotalRequests;

    return info;
}

/** Logs the receiving of the imported request in the imported log file. 
 *  Blocks if the log file is in use
 */
void logRequestReceived(char* logFilePath, sem_t* logFileSem, Request* request, int requestNo)
{
    sem_wait(logFileSem); //Getting lock on log file

    FILE* logFile = fopen(logFilePath, "a");

    if (logFile != NULL)
    {
        //Write to log file
        fprintf(logFile, "---------------------------------------------\n");
        fprintf(logFile, "  New Lift Request From Floor %d to Floor %d\n", request->start, request->dest);
        fprintf(logFile, "  Request No: %d\n", requestNo);
        fprintf(logFile, "---------------------------------------------\n");
        fprintf(logFile, "\n");
        
        fclose(logFile);
    }
    else
    {
        printf("LiftR: Failed to open log file! No logs written.\n"); //DEBUG
    }

    sem_post(logFileSem); //Releasing lock on log file

}