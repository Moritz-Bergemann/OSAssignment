#ifndef REQUEST_H
#define REQUEST_H

#include <pthread.h>

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
    pthread_mutex_t* mutex; // Mutex lock on buffer contents (pointer as that is how mutex is handled)
    pthread_cond_t* emptyCond; //Mutex condition used to let processes wait when buffer is empty
    pthread_cond_t* fullCond; //Mutex condition for waiting when buffer full
} RequestBuffer;


//Function headers//
RequestBuffer* createRequestBuffer(int size);
void freeRequestBuffer(RequestBuffer* buffer);
void addRequestToBuffer(Request* request, RequestBuffer* buffer);
Request* getRequestFromBuffer(RequestBuffer* buffer);


#endif