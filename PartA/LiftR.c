#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

/**LiftR by Moritz Bergemann
 * File containing functionality for reading lift requests from the imported file writing these to the request buffer.
 */

void *request(void* liftReqInfoVoid)
{
    printf("Hi, I'm the elevator request handler\n");

    pthread_exit(NULL);
}

