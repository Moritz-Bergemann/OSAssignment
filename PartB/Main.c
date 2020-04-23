#include "Main.h"
#include "Request.h"
#include "LiftR.h"
#include "Lift.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>
#include <wait.h>
#include <sys/mman.h>

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
                    manageProcesses(bufferSize, serviceLength);
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

    return 0;
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

/** Splits main process into sub-processes and calls their relevant methods. Performs resource
 *  preparation and cleanup. Creates shared memory between processes
 */
void manageProcesses(int bufferSize, int serviceLength)
{
    //Validating request file exists & contains correct number of requests

    //REQUEST BUFFER OPERATIONS
    //Creating and initialising request buffer
    RequestBuffer* reqBuffer;
    reqBuffer = createRequestBuffer(bufferSize);

    //Opening requirements & log files
    FILE* reqFile = fopen(INPUT_FILE_PATH, "r");
    FILE* logFile = fopen(OUTPUT_FILE_PATH, "w"); //Opening log file with 'write' flag to clear any previous contents

    //Create semaphore for log file
    sem_t *logSem = (sem_t*)createSharedMemory(sizeof(sem_t));
    sem_init(logSem, 1, 1); /*Creating log as binary semaphore (only 1 logfile resource) 
        initialised to be shared betweeen processes*/

    //Setting up information for request process
    LiftRequestProcessInfo* liftRInfo;
    liftRInfo = createReqProcessInfo(reqBuffer, INPUT_FILE_PATH, OUTPUT_FILE_PATH, logSem);
    
    //Setting up information for lift processes
    LiftProcessInfo* liftInfoArr[NUM_LIFTS];
    for (int ii = 0; ii < NUM_LIFTS; ii++)
    {
        liftInfoArr[ii] = createLiftProcessInfo(reqBuffer, (ii + 1), serviceLength, OUTPUT_FILE_PATH, logSem);
    }

    if (reqFile != NULL && logFile != NULL) //If requirements file opened successfully
    {
        //Closing log file (so can be reopened by processes when logging)
        fclose(logFile);
        
        //Creating & running processes

        int success = 1;
        
        //Create and run lift request handler process
        pid_t liftRPid = fork(); //Forking this method into 2 processes
        if (liftRPid < 0) //If fork resulted in error
        {
            printf("Error Occured: Failed to create request handler process\n");
            success = 0;
        }
        else if (liftRPid == 0) //If this is the created child process (i.e. Lift request handler process)
        {
            //Run lift request handler functionality
            request(liftRInfo);
            
            //Exit process (important so that rest of manageProcesses() method not run inside process)
            exit(0);
        }

        //Create and run lift processes
        pid_t liftPidArr[NUM_LIFTS];
        for (int ii = 0; ii < NUM_LIFTS; ii++)
        {
            liftPidArr[ii] = fork();
            if (liftPidArr[ii] < 0) //If process creation returned an error
            {
                printf("Error Occured: Failed to create lift process\n");
                success = 0;
            }
            else if (liftPidArr[ii] == 0) //If this is the child process just created
            {
                //Run lift functionality
                lift(liftInfoArr[ii]);

                /*Exit process (Important so that rest of manageProcesses() is not run in process
                    AND so that the loop does not cause each child process to fork into its own
                    children)*/
                exit(0);
            } //Else this is the parent process, pidArr[] will now contain the child's PID
        }
    
        if (success) //If all processes created successfully DEBUG
        {
            printf("All processes created successfully!\n");

            //Wait for all processes to complete
            pid_t completePid;
            for (int ii = 0; ii < NUM_LIFTS + 1; ii++)
            {
                //Waiting for process to complete, returns PID of completed process
                completePid = wait(NULL);

                if (completePid == liftRPid)
                {
                    printf("Main: Lift request handler opration complete!\n"); //DEBUG
                }
                else if (contains(completePid, liftPidArr, NUM_LIFTS))
                {
                    //Getting lift number of lift process that just completed
                    int arrPos = -1;
                    int found = 0;
                    while (arrPos < NUM_LIFTS && !found)
                    {
                        arrPos++;
                        if (liftPidArr[arrPos] == completePid)
                        {
                            found = 1;
                        }
                    }
                    if (found)
                    {
                        printf("Main: Lift-%d operation complete!\n", liftInfoArr[arrPos]->liftNum);
                    }
                    else
                    {
                        printf("Error: Lift with unknown number complete!\n");
                    }
                    
                }
                else 
                {
                    printf("Error - undefined process complete?!\n"); //DEBUG
                }
            }
        }
        else
        {
            printf("Error: failed to create all processes\n");
        }
    }
    else
    {
        printf("Error: failed to open request and/or log file. Operations aborted.\n");
    }

    //Performing cleanup
    fclose(reqFile); //Closing requests file
    sem_destroy(logSem);
    munmap(logSem, sizeof(sem_t));

    freeRequestBuffer(reqBuffer); //Freeing requests buffer using custom method

    //Freeing process info structs
    free(liftRInfo);
    for (int ii = 0; ii < NUM_LIFTS; ii++)
    {
        freeLiftProcessInfo(liftInfoArr[ii]);
    }

    printf("Main: Exiting...\n");
}

/** Returns true if the imported integer is contained in the imported integer array, 
 *  and false otherwise.
 */
int contains(int num, int* array, int arraySize)
{
    int contained = 0;

    for (int ii = 0; ii < arraySize; ii++)
    {
        if (array[ii] == num)
        {
            contained = 1;
        }
    }

    return contained;
}