#ifndef REQUEST_H
#define REQUEST_H

#include <math.h>
#include <semaphore.h>

//Preprocessor Constants//
#define BUFFER_TIMEOUT_S 0 //Number of seconds request retrieval waits before timing out
#define BUFFER_TIMEOUT_NS  ((int)(5 * powl(10, 8))) //Number of nanoseconds request retrieval waits before timing out (10^8 nanoseconds = 0.1 seconds)

//Structs//
//Lift request
typedef struct {
    int start; //Starting floor
    int dest; //Destination floor
} Request;

//Buffer of lift requests
typedef struct {
    Request* reqQueue; //Queue (i.e. Array with FIFO handling) of (pointers to) buffer requests
    int size; //Buffer size
    int used; //Number of requests currently in buffer
    int done; //Tracks whether all requests have been completed
    sem_t* sem; // Binary semaphore lock on buffer contents (pointer as that is how semaphore is handled)
    sem_t* fullSem; //When this semaphore empty, buffer is empty
    sem_t* emptySem; //When this semaphore empty, buffer is full
} RequestBuffer;


//Function headers//
RequestBuffer* createRequestBuffer(int size);
void* createSharedMemory(int size);
void freeRequestBuffer(RequestBuffer* buffer);
void addRequestToBuffer(Request* request, RequestBuffer* buffer);
Request* getRequestFromBuffer(RequestBuffer* buffer);
void markDone(RequestBuffer* buffer);

#endif