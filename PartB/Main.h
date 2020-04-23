#ifndef MAIN_H
#define MAIN_H

#include "Lift.h"
#include <stdio.h>

//Preprocessor constants//
#define MIN_REQUESTS 50
#define MAX_REQUESTS 100
#define MAX_SERVICE_LENGTH  100
#define NUM_FLOORS 20
#define INPUT_FILE_PATH "sim_input.txt"
#define OUTPUT_FILE_PATH "sim_output.txt"
#define NUM_LIFTS 3

//Function headers//
int main(int argc, char** argv);
void printHelp();
void manageProcesses(int bufferSize, int serviceLength);
void waitForProcesses(pid_t* liftPidArr, pid_t liftRPid, LiftProcessInfo** liftInfoArr);
int contains(int num, int* array, int arraySize);
#endif