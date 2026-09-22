#include <stdio.h>
#include <stdlib.h>

/* Your `schedule` program should fork a series of child processes,
a child process for each process defined in an input file, adding
them into a priority queue.
A process with a higher priority (lower number) should be running
to completion.
Processes with the same priority should circulate, and run in
time quantum intervals using Round Robin. When a process
concludes, it should be taken out of circulation. */

int main(int argc, char * argv[]){

/* 2 params */
    //1st param = time quant
    int time_quant = atoi(argv[1]);
    if(time_quant == NULL){
        printf("Invalid time\n");
        exit(1);
    }

    //file reader
    FILE *fp;
    fp = fopen(argv[2], "r");
    if(fp == NULL){
        printf("Invalid file input\n");
        exit(1);
    }
    /* a process identifier: any unique natural number.
    - a process priority: natural number from 0 to 127, with lower
    values indicating higher priority.
    - a process binary file
    - and optional parameters for the binary file*/

    fclose(fp);



}
