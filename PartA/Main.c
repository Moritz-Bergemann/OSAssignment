#include "Main.h"
#include "Request.h"
#include "LiftR.h"
#include "Lift.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>


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
        printHelp();
    }
}

/** Outputs help for program
 */
void printHelp()
{
    printf("Lift simulation A by Moritz Bergemann:\n");
    printf("Run in format \"lift_sim_A m t\"\n");
    printf("\t m: Buffer size (must be between 1 & %d)\n", MAX_REQUESTS);
    printf("\t t: Time required for lift service (must be between 1 & %d)\n", MAX_SERVICE_LENGTH);
}

/** Initiates threads TODO more info here
 */
void manageThreads(int bufferSize, int serviceLength)
{
    //Validating request file exists & contains correct number of requests

    //Creating and initialising request buffer
    RequestBuffer* reqBuffer;
    reqBuffer = createRequestBuffer(bufferSize);

    //Opening requirements & log files & creating mutex for log file
    FILE* reqFile = fopen(INPUT_FILE_PATH, "r");
    FILE* logFile = fopen(OUTPUT_FILE_PATH, "w");
    pthread_mutex_t* logFileMutex;
    pthread_mutex_init(logFileMutex, NULL);

    //Setting up information for request process
    LiftRequestThreadInfo* reqInfo;
    reqInfo = createReqThreadInfo(reqBuffer, reqFile, logFile, logFileMutex);

    //Setting up information for 3 lift processes
    LiftThreadInfo* liftInfo1, *liftInfo2, *liftInfo3;
    liftInfo1 = createLiftThreadInfo(reqBuffer, 1, serviceLength, logFile, logFileMutex);
    liftInfo2 = createLiftThreadInfo(reqBuffer, 2, serviceLength, logFile, logFileMutex);
    liftInfo3 = createLiftThreadInfo(reqBuffer, 3, serviceLength, logFile, logFileMutex);

    if (reqFile != NULL && logFile != NULL) //If both required files opened successfully
    {
        //Creating threads
        pthread_t liftR, lift1, lift2, lift3;
        int success = 0;

        success += pthread_create(&liftR, NULL, request, (void*)reqInfo);
        success += pthread_create(&lift1, NULL, lift, (void*)liftInfo1);
        success += pthread_create(&lift2, NULL, lift, (void*)liftInfo2);
        success += pthread_create(&lift3, NULL, lift, (void*)liftInfo3);
        
        if (success == 0) //If all threads created successfully DEBUG
        {
            printf("All threads created successfully!\n");
        }
        else
        {
            printf("Fatal error - failed to create all threads\n");
        }
    }
    else
    {
        printf("Error - failed to open request and/or log file. Operations aborted.\n");
    }

    //Performing cleanup
    fclose(reqFile); //Closing requests file
    freeRequestBuffer(reqBuffer); //Freeing requests buffer using custom method

    //Freeing thread info structs
    free(reqInfo);
    free(liftInfo1);
    free(liftInfo2);
    free(liftInfo3);

    printf("Exiting...\n");
}

/** Checks imported filename points to a valid file with the correct number of requests
 *  (though it does not check whether these requests are valid)
 */
FILE* checkFile(char* path)
{
    FILE* file;

    file = fopen(path, "r'");

    if (file == NULL)
    {
        fatalError("Failed to open requests file");;
    }

    return file;
}

//TODO refactor so this isn't needed, it really doesn't work when you need cleanup and thread safety
void fatalError(char* message)
{
    printf("Fatal Error: %s\n", message);
    printf("Exiting...\n");
    exit(1); //Exit with error code 1
}