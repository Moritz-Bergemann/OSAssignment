#ifndef MAIN_H
#define MAIN_H

#include <stdio.h>

//Preprocessor constants//
#define MIN_REQUESTS 50
#define MAX_REQUESTS 100
#define MAX_SERVICE_LENGTH 100
#define NUM_FLOORS 20
#define INPUT_FILE_PATH "sim_input.txt"
#define OUTPUT_FILE_PATH "sim_output.txt"
#define NUM_LIFTS 3

//Defining DEBUG_PRINT macro (will only print out to terminal if 'DEBUG' flag defined during compilation)
#ifdef DEBUG
#define DEBUG_PRINT(...) (printf(__VA_ARGS__)) //Print to terminal
#else
#define DEBUG_PRINT(...) do { } while (0) //do nothing
#endif

//Function headers//
int main(int argc, char** argv);
void printHelp();
void manageThreads(int bufferSize, int serviceLength);
#endif