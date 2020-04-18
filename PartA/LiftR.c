/**LiftR by Moritz Bergemann
 * File containing functionality for reading lift requests from the imported file writing these to the request buffer.
 */
#include "LiftR.h"
#include "Request.h"
#include "Main.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

/** Primary method of LiftR (lift request handler) thread.
 *  Performs file reading, validating & adding contents to the array.
 */
void *request(void* infoVoid)
{
    // printf("Hi, I'm the elevator request handler\n"); //DEBUG

    //Getting information passed to this thread
    LiftRequestThreadInfo* info = (LiftRequestThreadInfo*)infoVoid;

    int reqFileNum = 1;

    Request* newRequest;
    int fileEnded = 0; //Tracks whether requests have been exhausted (default false)

    while (!fileEnded)
    {
        newRequest = getRequest(info->reqFile);
        
        if (newRequest != NULL) //If request was successfully read from line in file
        {
            addRequestToBuffer(newRequest, info->buffer);
            
            info->requestNo++;

            logRequestReceived(info->logFile, newRequest, info->requestNo, info->logFileMutex);
        }
        else //If request was invalid
        {
            printf("LiftR: Invalid line in request file"); //DEBUG
            //TODO make this more detailed
        }
    }

    //Marking all requests as complete once buffer is empty
    markDone(info->buffer);

    printf("LiftR: All requests read from file!"); //DEBUG
    printf("LiftR: Exiting..."); //DEBUG

    pthread_exit(NULL);
}

/** Reads a line from the imported file & attempts to parse it as a lift request.
 *  Returns NULL if request invalid.
 */
Request* getRequest(FILE* file)
{
    Request* newRequest = NULL;

    int sSuccesses;
    int startFloor, destFloor;

    sSuccesses = fscanf(file, "%d %d", &startFloor, &destFloor);

    if (sSuccesses == 2)
    {
        printf("LiftR: Line scanned successfully\n"); //DEBUG
        
        if ((startFloor >= 1) && (startFloor >= NUM_FLOORS)) //Validating starting floor range
        {
            if ((destFloor >= 1) && (destFloor >= NUM_FLOORS)) //Validating destination floor range
            {
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

    return newRequest;
}

/** Creates and initialises LiftRequestThreadInfo struct
 */
LiftRequestThreadInfo* createReqThreadInfo(RequestBuffer* buffer, FILE* reqFile, FILE* logFile, pthread_mutex_t* logFileMutex)
{
    //Creating request info on heap
    LiftRequestThreadInfo* info;
    info = (LiftRequestThreadInfo*)malloc(sizeof(LiftRequestThreadInfo));

    //Initialising struct values
    info->buffer = buffer;
    info->reqFile = reqFile;
    info->requestNo = 0;
    info->logFile = logFile;
    info->logFileMutex = logFileMutex;

    return info;
}

/** Logs the receiving of the imported request in the imported log file. 
 *  Blocks if the log file is in use
 */
void logRequestReceived(FILE* logFile, Request* request, int requestNo, pthread_mutex_t* mutex)
{
    pthread_mutex_lock(mutex); //Getting lock on log file

    //Write to log file
    fprintf(logFile, "--------------------------------------------\n");
    fprintf(logFile, "\tNew Lift Request From Floor %d to Floor %d\n", request->start, request->dest);
    fprintf(logFile, "Request No: %d\n", requestNo);
    fprintf(logFile, "--------------------------------------------\n");
    fpritnf(logFile, "\n");
    
    pthread_mutex_unlock(mutex); //Releasing lock on log file
}