#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include "queue.h"
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
    char *buf = NULL;
    size_t len = 0;
	ssize_t read;
    struct PriorityQueue queue1;


	while((read = getline(&buf, &len, fp)) != -1){
        struct Process p1;
		p1.pid = atoi(buf[0]);
		p1.priority = atoi(buf[1]);	
		p1.filename = buf[2]; //binary file
        if(len == 3){
            p1.params2 = atoi(buf[3]);
        } else if(len > 3){
    		p1.params1 = buf[3]; //TODO: fix this param later	 
            p1.params2 = atoi(buf[3]);           
        }
        //add to pq
        enqueue(queue1, p1);					  	
	}	

	// for loop, fork depending on line, go back to it or smth
	pid_t pid = fork();
	if(pid < 0){
		printf("fork failed");
		exit(1);
	} else if (pid == 0){
		
	}

	free(line);	
	fclose(fp);
    return 0;

}
