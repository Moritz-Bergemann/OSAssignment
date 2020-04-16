#ifndef MAIN_H
#define MAIN_H

//Preprocessor constants//
#define MIN_REQUESTS 50
#define MAX_REQUESTS 100
#define MAX_SERVICE_LENGTH  100
#define REQUEST_FILE_NAME "sim_input.txt"
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

//Information for the request thread
typedef struct {
    FILE* reqFile; //File containing lift requests
    RequestBuffer* buffer; //Lift request buffer
} LiftRequestThreadInfo;

//Information for the lift process
typedef struct {
    int liftNum; //Identifying number of this lift (probably 1-3)
    RequestBuffer* buffer; //Lift request buffer
} LiftThreadInfo;

//Function headers//
int main(int argc, char** argv);
void printHelp();
void manageThreads(int bufferSize, int serviceLength);
RequestBuffer* createRequestBuffer(int size);
LiftRequestThreadInfo* createReqThreadInfo(RequestBuffer* buffer, FILE* reqFile);
LiftThreadInfo* createLiftThreadInfo(RequestBuffer* buffer, int liftNum);
FILE* checkFile(char* path);
void fatalError(char* message);
#endif