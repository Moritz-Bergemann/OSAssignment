#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "Main.h"

/** Takes in & validates command line parameters
 */
int main(int argc, char** argv)
{
    if (argc == 3)
    {
        //Getting & validating command line parameters
        int bufferSize, serviceLength;
        int sSuccesses = 0;
        sSuccesses += sscanf(argv[1], "%d", &bufferSize);
        sSuccesses += sscanf(argv[2], "%d", &serviceLength);
        if (sSuccesses == 2)
        {
            if (bufferSize >= 1 && bufferSize <= MAX_REQUESTS) //Validating buffer size range
            {
                if (serviceLength >= 0 && serviceLength <= MAX_SERVICE_LENGTH) //Validating service length range
                {
                    //All settings valid, begin running program
                    manageThreads(bufferSize, serviceLength);
                }
                else
                {
                    printf("Error: Service duration must be between 1 & %d (inclusive)! Exiting...\n", MAX_SERVICE_LENGTH);
                }
                
            }
            else
            {
                printf("Error: Buffer size must be between 1 & %d (inclusive)! Exiting...\n", MAX_REQUESTS);
            }
            
        }
        else
        {
            printf("Incorrect command line argument format (both must be whole numbers)!\n");
        }
        
    }
    else
    {
        printf("Incorrect number of command line arguments (2 required)!\n");
        printHelp();
    }
}

/** Outputs help for program
 */
void printHelp()
{
    printf("Lift simulation A by Moritz Bergemann:\n");
    printf("Run in format \"lift_sim_A m t\"");
    printf("\t m: Buffer size (must be between 1 & %d)\n", MAX_REQUESTS);
    printf("\t t: Time required for lift service (must be between 1 & %d)\n", MAX_SERVICE_LENGTH);
}

/** Initiates threads TODO more info here
 */
void manageThreads(int bufferSize, int serviceLength)
{
    //Validating request file exists & contains correct number of requests
    FILE* reqFile = checkFile(REQUEST_FILE_NAME);

    //Creating request buffer
    int* reqBuffer = (int*)malloc(sizeof(int) * bufferSize);

    //Setting up information for request process
    LiftRequestProcessInfo* reqThreadInfo;
    reqThreadInfo = makeReqThreadInfo(bufferSize, reqBuffer, reqFile);

    //Setting up information for 3 lift processes

    pthread_t liftR;
    int success;

    success = pthread_create(&liftR, NULL, request(), (void*)reqFile);
}

LiftRequestProcessInfo* makeReqThreadInfo(int bufferSize, int* buffer, FILE* reqFile)
{
    LiftRequestProcessInfo* info;

    //Creating request info on heap
    info = (LiftRequestProcessInfo*)malloc(sizeof(LiftRequestProcessInfo));

    //Allocating struct values
    info->bufferSize = bufferSize;
    info->buffer = buffer;
    info->reqFile = reqFile;
}


/** Checks imported filename points to a valid file with the correct number of requests
 *  (though it does not check whether these requests are valid)
 */