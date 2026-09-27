#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include "queue.h"
#include <signal.h>
#include <sys/time.h>
/* Your `schedule` program should fork a series of child processes,
a child process for each process defined in an input file, adding
them into a priority queue.
A process with a higher priority (lower number) should be running
to completion.
Processes with the same priority should circulate, and run in
time quantum intervals using Round Robin. When a process
concludes, it should be taken out of circulation. */

// void timer_callback(int signum){
//     printf("timer done");
// }

int main(int argc, char * argv[]){

/* 2 params */
    //1st param = time quant
    int time_quant = atoi(argv[1]);
    if(!time_quant){
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
    PriorityQueue queue1;

	while((read = getline(&buf, &len, fp)) != -1){
        
        //TODO: error handling
        Process p1;
        p1.pid = buf[0] - '\0';
        p1.priority = buf[1] - '\0';	
        p1.filename = buf[2]; //binary file, have to exec it
        if(len == 3){
            p1.bursttime = buf[3] - '\0';
            p1.params1 = NULL;
        } else if(len > 3){
            p1.params1 = buf[3]; // this param goes w/ program
            p1.bursttime = buf[3] - '\0';           
        }
        //add to pq
        enqueue(queue1, p1.priority, p1);					  	
	}	

	free(buf);	

    /*Processes with the same priority should circulate, 
    and run in time quantum intervals using Round Robin*/

    Process curr = dequeue(queue1);
    while (curr){
        pid_t pid = fork();
        if(pid < 0){
            perror("fork failed");
            return -1;
        }else if (pid == 0){
            //run it for time quantum using setittimer()
            if(curr.params1 == NULL){
                char *args = {curr.filename, NULL};            
            } else if(curr.params1 != NULL){
                char *args = {curr.filename, curr.params1, NULL};
            }
            execvp(args[0], args);
            //TODO: timer here
            //TODO: if over time quantum, then enqueue again
            
            perror("execvp failed");
            return -1; 
        }else{
            wait(NULL);
        }
    }
    
	fclose(fp);
    return 0;

}
