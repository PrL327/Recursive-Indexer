#include "Indexer.h"
#include <stdio.h>
#include <time.h>

/*
 * Function: main
 *-----------------------
 *   simply checks number of arguments and if there is
 *   a correct amount of arguments, it starts indexing
 *
 *   argc: number of arguments
 *   argv: array of the arguments
 *
 *   returns: 0 (if its successful), -1 if something fails
 */
int main(int argc, char* argv[])
{
    if(argc < 3) //if less than 3 args
    {
        printf("Error: not enough arguments!\n");
    }
    else if(argc > 3) //if more than 3 args
    {
        printf("Error: too many arguments!\n");
    }
    else //correct amount of args
    {
        clock_t start = clock(); //start clock
        int status = dirMain(argv[1], argv[2]);
        clock_t end = clock(); //end clock

        double total_time = (double)(end - start)/CLOCKS_PER_SEC;

        printf("Time:%f  seconds\n",total_time);
        return status;
    }
    return 0;
}
