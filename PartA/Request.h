#ifndef REQUEST_H
#define REQUEST_H

//Structs//
//Lift request
typedef struct {
    int start; //Starting floor
    int end; //Ending floor
} Request;

//Buffer of lift requests
typedef struct {
    Request** reqArray; //Array of (pointers to) buffer requests
    int size; //Buffer size
    int used; //Number of requests currently in buffer
} RequestBuffer;


//Function headers//
RequestBuffer* createRequestBuffer(int size);
void freeRequestBuffer(RequestBuffer* buffer);

#endif