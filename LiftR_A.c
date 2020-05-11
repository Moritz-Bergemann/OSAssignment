/**LiftR by Moritz Bergemann
 * File containing functionality for reading lift requests from the imported file writing these to the request buffer.
 */
#include "LiftR_A.h"
#include "Request_A.h"
#include "Main_A.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

/** Primary method of LiftR (lift request handler) thread.
 *  Performs file reading, validating & adding contents to the array.
 */
void *request(void* infoVoid)
{
    //Getting information passed to this thread
    LiftRequestThreadInfo* info = (LiftRequestThreadInfo*)infoVoid;

    Request* newRequest = NULL;
    int fileEnded = 0; //Tracks whether requests have been exhausted (default false)
    int lineNum = 0; //Tracks the current line number (for error messages)

    while (!fileEnded)
    {
        lineNum++;

        //Getting request from file (placed into pointer, returns whether EOF reached)
        fileEnded = getRequest(info->reqFile, &newRequest);
        
        if (newRequest != NULL) //If request was successfully read from line in file
        {
            addRequestToBuffer(newRequest, info->buffer);

            info->requestNo++;

            logRequestReceived(info->logFile, newRequest, info->requestNo, info->logFileMutex);
        }
        else //If request was invalid
        {
            DEBUG_PRINT("LiftR: Invalid line in request file - line %d\n", lineNum); //DEBUG
        }
    }

    //Marking all requests as complete once buffer is empty
    markDone(info->buffer);

    DEBUG_PRINT("LiftR: All requests read from file!\n"); //DEBUG
    DEBUG_PRINT("LiftR: Exiting...\n"); //DEBUG

    pthread_exit(NULL);
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
        DEBUG_PRINT("LiftR: Line scanned successfully\n"); //DEBUG
        
        if ((startFloor >= 1) && (startFloor <= NUM_FLOORS)) //Validating starting floor range
        {
            if ((destFloor >= 1) && (destFloor <= NUM_FLOORS)) //Validating destination floor range
            {
                //Creating request struct & giving it the validated values
                newRequest = (Request*)malloc(sizeof(Request));
                newRequest->start = startFloor;
                newRequest->dest = destFloor;
            }
            else
            {
                DEBUG_PRINT("LiftR: Destination floor not in valid range\n");
            }
            
        }
        else
        {
            DEBUG_PRINT("LiftR: Starting floor not in valid range\n");
        }
    }
    else if (sSuccesses == EOF)
    {
        DEBUG_PRINT("LiftR: Reached end of file!\n"); //Debug
        fileEnded = 1;
    }
    else
    {
        DEBUG_PRINT("LiftR: Failed to read line!\n"); //Debug
    }
    
    *requestAddr = newRequest;
    return fileEnded;
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
    fprintf(logFile, "---------------------------------------------\n");
    fprintf(logFile, "  New Lift Request From Floor %d to Floor %d\n", request->start, request->dest);
    fprintf(logFile, "  Request No: %d\n", requestNo);
    fprintf(logFile, "---------------------------------------------\n");
    fprintf(logFile, "\n");
    
    pthread_mutex_unlock(mutex); //Releasing lock on log file
}