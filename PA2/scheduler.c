#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include "queue.h"
#include <signal.h>
#include <sys/time.h>
#include <string.h>
#include <sys/wait.h>

/* Your `schedule` program should fork a series of child processes,
a child process for each process defined in an input file, adding
them into a priority queue.
A process with a higher priority (lower number) should be running
to completion.
Processes with the same priority should circulate, and run in
time quantum intervals using Round Robin. When a process
concludes, it should be taken out of circulation. */

void timer_handler(int signum){
    (void)signum;
}

int main(int argc, char * argv[]){

    if(argc != 3){
        printf("not enough params");
        exit(1);
    }
/* 2 params */
    //1st param = time quant
    int time_quant = atoi(argv[1]);
    if(!time_quant){
        printf("Invalid time\n");
        exit(1);
    }

    //timer
    struct sigaction sa;
    struct itimerval timer;
    sa.sa_handler = &timer_handler;
    sigaction(SIGALRM, &sa, NULL);

    struct itimerval timer;
    //how many sec + uc
    timer.it_value.tv_sec = time_quant/1000; //TODO: check how to break up timer into sec + milliseconds
    timer.it_value.tv_usec = 0;
    //num of intervals
    timer.it_interval.tv_sec = 128; //TODO: fix this number
    timer.it_interval.tv_usec = 0;
    setittimer(ITIMER_REAL, &timer, NULL);


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
    queue1.size = 0;

	while((read = getline(&buf, &len, fp)) != -1){
        
	    Process p1;
        char *token = strtok(buf, " \t\n");
        int i = 0;  
        char *array[6];

        //get all six items
        while(token != NULL && i < 6){
            array[i] = token;
            token = strtok(NULL, " \t\n");
            i++;
        }
        //parse through depending on token length
        if(i == 4){
            p1.pid = atoi(array[0]);
            p1.priority = atoi(array[1]);
            p1.filename = strdup(array[2]);
            p1.bursttime = atoi(array[3]);
            p1.params1 = NULL;
        } else if(i == 5){
            p1.pid = atoi(array[0]);
            p1.priority = atoi(array[1]);
            p1.filename = strdup(array[2]);
            p1.bursttime = atoi(array[3]);

            //strip quotes in params1
            char *quotes_str = array[4];
            int len_params1 = strlen(quotes_str);
            if(len_params1 > 0 && quotes_str[len_params1 - 1] == '"'){
                quotes_str[len_params1 -1] = '\0';
                len_params1--;
            }
            if(len_params1 > 0 && quotes_str[0] == '"'){
                quotes_str++;
            }

            p1.params1 = strdup(quotes_str);
        } else{
            printf("invalid line\n");
            return -1;
        }

        //add to pq
        enqueue(&queue1, p1.priority, p1);					  	
	}	

	free(buf);	

    /*Processes with the same priority should circulate, 
    and run in time quantum intervals using Round Robin
    
    9. dynamically allocate and deallocate resources for each
    process.
    4. allow each process to run until either the quantum expires or
    it terminates or it suspends itself.*/

    while (peek(&queue1) != -1){
        Process curr = dequeue(&queue1);
        pid_t pid = fork();
        if(pid < 0){
            perror("fork failed");
            return -1;
        } else if (pid == 0){
            //run it for time quantum using setittimer()
            char path[128];
            snprintf(path, sizeof(path), "./%s", curr.filename);
            char *args[3] =  {path, curr.params1, NULL};
            printf("%s %s\n", curr.filename, curr.params1);
            
            //timer pause?
            pause();

            execvp(args[0], args);
            //TODO: if over time quantum, then enqueue again
            
            perror("execvp failed");
            return -1; 
        } else{
            wait(NULL);
        }
    }
    
	fclose(fp);
    return 0;

}
