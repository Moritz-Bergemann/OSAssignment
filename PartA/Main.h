#ifndef MAIN_H
#define MAIN_H

#include <stdio.h>

//Preprocessor constants//
#define MIN_REQUESTS 50
#define MAX_REQUESTS 100
#define MAX_SERVICE_LENGTH  100
#define NUM_FLOORS 20
#define INPUT_FILE_PATH "sim_input.txt"
#define OUTPUT_FILE_PATH "sim_output.txt"

//Function headers//
int main(int argc, char** argv);
void printHelp();
void manageThreads(int bufferSize, int serviceLength);
FILE* checkFile(char* path);
void fatalError(char* message);
#endif