#include <stdio.h>
#include <stdlib.h>


/**Lift-R by Moritz Bergemann
 * File containing functionality for reading lift requests from the imported file writing these to the request buffer.
 */

void request(FILE* file, int bufferSize)
{
    FILE* file;

    file = fopen(filename, "r");

    if (file != NULL) //If file successfully opened
    {
        
    }
    else //If file failed to open
    {
        printf("Error - failed to open file")
    }
    

    
}

/** Checks imported filename points to a valid file with the correct number of requests
 *  (though it does not check whether these requests are valid)
 */
FILE* fileCheck(char** path)
{

}