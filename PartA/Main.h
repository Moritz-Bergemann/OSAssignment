#ifndef MAIN_H
#define MAIN_H

//Preprocessor constants//
#define MIN_REQUESTS 50
#define MAX_REQUESTS 100
#define MAX_SERVICE_LENGTH  100
#define REQUEST_FILE_NAME "sim_input"
#define NUM_FLOORS = 20;

//Structs//

//Lift request
typedef struct {
    int start; //Starting floor
    int end; //Ending floor
} Request;

//Buffer of lift requests
typedef struct {
    Request* reqArray; //Array of buffer requests
    int size; //Buffer size
    int used; //Number of requests currently in buffer
} RequestBuffer;

//Information for the request process
typedef struct {
    FILE* reqFile; //File containing lift requests
    RequestBuffer* buffer; //Lift request buffer
} LiftRequestProcessInfo;

//Information for the lift process
typedef struct {
    int liftNumber; //Identifying number of this lift (probably 1-3)
    RequestBuffer* buffer; //Lift request buffer
} LiftProcessInfo;

//Function headers//
int main(int argc, char** argv);
#endif