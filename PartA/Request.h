#ifndef REQUEST_H
#define REQUEST_H

#include <pthread.h>
#include <math.h>

//Preprocessor Constants//
#define BUFFER_TIMEOUT_S 0 //Number of seconds request retrieval waits before timing out
#define BUFFER_TIMEOUT_NS (5 * powl(10, 8)) //Number of nanoseconds request retrieval waits before timing out

//Structs//
//Lift request
typedef struct {
    int start; //Starting floor
    int dest; //Destination floor
} Request;

//Buffer of lift requests
typedef struct {
    Request** reqArray; //Array of (pointers to) buffer requests
    int size; //Buffer size
    int used; //Number of requests currently in buffer
    int done; //Tracks whether all requests have been completed
    pthread_mutex_t* mutex; // Mutex lock on buffer contents (pointer as that is how mutex is handled)
    pthread_cond_t* addedCond; //Mutex condition used to let processes wait when buffer is empty
    pthread_cond_t* removedCond; //Mutex condition for waiting when buffer full
} RequestBuffer;


//Function headers//
RequestBuffer* createRequestBuffer(int size);
void freeRequestBuffer(RequestBuffer* buffer);
void addRequestToBuffer(Request* request, RequestBuffer* buffer);
Request* getRequestFromBuffer(RequestBuffer* buffer);
void markDone(RequestBuffer* buffer);

#endif