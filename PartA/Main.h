#ifndef MAIN_H
#define MAIN_H

#include <stdio.h>

//Preprocessor constants//
#define MIN_REQUESTS 50
#define MAX_REQUESTS 100
#define MAX_SERVICE_LENGTH  100
#define REQUEST_FILE_NAME "sim_input.txt"
#define NUM_FLOORS 20


//Function headers//
int main(int argc, char** argv);
void printHelp();
void manageThreads(int bufferSize, int serviceLength);
FILE* checkFile(char* path);
void fatalError(char* message);
#endif