#include "Main_A.h"
#include "Request_A.h"
#include "LiftR_A.h"
#include "Lift_A.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

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
                    printf("Error: Service duration must be between 0 & %d (inclusive)! Exiting...\n", MAX_SERVICE_LENGTH);
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

    return 0;
}

/** Outputs help for program
 */
void printHelp()
{
    printf("Lift simulation A by Moritz Bergemann:\n");
    printf("Run in format \"lift_sim_A m t\"\n");
    printf("\t m: Buffer size (must be between 1 & %d)\n", MAX_REQUESTS);
    printf("\t t: Time required for lift service (must be between 0 & %d)\n", MAX_SERVICE_LENGTH);
}

/** Primary method, gets necessary information, creates threads & passes them
 *  their information. Performs memory reservation and cleanup for this info.
 */
void manageThreads(int bufferSize, int serviceLength)
{
    //Creating and initialising request buffer
    RequestBuffer* reqBuffer;
    reqBuffer = createRequestBuffer(bufferSize);

    //Opening requirements & log files & creating mutex for log file
    FILE* reqFile = fopen(INPUT_FILE_PATH, "r");
    FILE* logFile = fopen(OUTPUT_FILE_PATH, "w");

    pthread_mutex_t* logFileMutex = (pthread_mutex_t*)malloc(sizeof(pthread_mutex_t));
    pthread_mutex_init(logFileMutex, NULL);

    //Setting up information for request thread
    LiftRequestThreadInfo* reqInfo;
    reqInfo = createReqThreadInfo(reqBuffer, reqFile, logFile, logFileMutex);

    //Setting up information for lift threads
    LiftThreadInfo* liftInfoArr[NUM_LIFTS];
    for (int ii = 0; ii < NUM_LIFTS; ii++)
    {
        liftInfoArr[ii] = createLiftThreadInfo(reqBuffer, (ii + 1), serviceLength, logFile, logFileMutex);
    }


    if (reqFile != NULL && logFile != NULL) //If both required files opened successfully
    {
        //Creating threads
        pthread_t liftR;
        pthread_t liftArr[NUM_LIFTS];
        int success = 0;

        success += pthread_create(&liftR, NULL, request, (void*)reqInfo);

        for (int ii = 0; ii < NUM_LIFTS; ii++)
        {
            success += pthread_create(&liftArr[ii], NULL, lift, (void*)liftInfoArr[ii]);
        }
        
        if (success == 0) //If all threads created successfully DEBUG
        {
            printf("All threads created successfully! Running...\n");

            //Waiting for all threads to complete
            pthread_join(liftR, NULL);
            printf("Lift request handler operation complete!\n");
            
            for (int ii = 0; ii < NUM_LIFTS; ii++)
            {
                pthread_join(liftArr[ii], NULL);
                printf("Lift-%d operation complete!\n", ii + 1);
            }

            //Writing final statistics to log file (no mutex needed since all threads complete)
            int totalRequests = reqInfo->requestNo;
            int totalMovement = 0;
            for (int ii = 0; ii < NUM_LIFTS; ii++)
            {
                totalMovement += liftInfoArr[ii]->totalMovement;
            }

            fprintf(logFile, "Total number of requests: %d\n", totalRequests);
            fprintf(logFile, "Total number of movements: %d", totalMovement); //No '\n' as end of file
        }
        else
        {
            printf("Error: failed to create all threads\n");
        }

        //Closing requests & log file
        fclose(reqFile); 
        fclose(logFile);
    }
    else
    {
        printf("Error: failed to open request and/or log file. Operations aborted.\n");
    }

    //Performing cleanup
    free(logFileMutex);

    freeRequestBuffer(reqBuffer); //Freeing requests buffer using custom method

    //Freeing thread info structs
    free(reqInfo);
    
    for (int ii = 0; ii < NUM_LIFTS; ii++)
    {
        freeLiftThreadInfo(liftInfoArr[ii]);
    }

    printf("Exiting...\n");
}