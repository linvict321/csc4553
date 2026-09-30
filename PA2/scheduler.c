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
#define PRIORITY_MULT 1000000
volatile sig_atmoic_t quantum_expired = 0;

void timer_handler(int signum){
    (void)signum;
    quantum_expired = 1;
}

void start_timer(int ms){
    struct itimerval timer;
    timer.it_value.tv_sec  = ms / 1000;
    timer.it_value.tv_usec = (ms % 1000) * 1000; /* remainder -> microseconds */
    timer.it_interval.tv_sec  = 0;
    timer.it_interval.tv_usec = 0;
    if (setitimer(ITIMER_REAL, &timer, NULL) == -1) {
        perror("setitimer");
    }
}

void cancel_timer(void){
    struct itimerval timer;
    timer.it_value.tv_sec = 0;
    timer.it_value.tv_usec = 0;
    timer.it_interval.tv_sec = 0;
    timer.it_interval.tv_usec = 0;
    setitimer(ITIMER_REAL, &timer, NULL);
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
    sa.sa_handler = &timer_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    if (sigaction(SIGALRM, &sa, NULL) == -1) {
        perror("sigaction");
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
    queue1.size = 0;
    int seq = 0;

	while((read = getline(&buf, &len, fp)) != -1){
        
	    Process p1;
        char *token = strtok(buf, " \t\n");
        int i = 0;  
        char *array[3 + MAX_PARAMS];

        //get all six items
        while(token != NULL && i < (3 + MAX_PARAMS)){
            array[i] = token;
            token = strtok(NULL, " \t\n");
            i++;
        }
        //parse through depending on token length
        if(i < 3){
            printf("invalid line\n");
            continue;
        }
 
        p1.pid = atoi(array[0]);
        p1.priority = atoi(array[1]);
        p1.filename = strdup(array[2]);
        p1.nparams = 0;
        p1.child_pid = -1; 

        for(int j = 3; j < i && p1.nparams < MAX_PARAMS; j++){
            char *tok = array[j];
            size_t tl = strlen(tok);
            if(tl > 0 && tok[tl - 1] == '"'){
                tok[tl - 1] = '\0';
                tl--;
            }
            if(tl > 0 && tok[0] == '"'){
                tok++;
            }
            p1.params[p1.nparams++] = strdup(tok);
        }
 
        //add to pq, keyed by (priority, arrival order)
        enqueue(&queue1, p1.priority * PRIORITY_MULT + seq++, p1);				  	
	}	

	free(buf);	
    fclose(fp);
    /*Processes with the same priority should circulate, 
    and run in time quantum intervals using Round Robin
    
    9. dynamically allocate and deallocate resources for each
    process.
    4. allow each process to run until either the quantum expires or
    it terminates or it suspends itself.*/

    while (peek(&queue1) != -1){
        Process curr = dequeue(&queue1);
        if(curr.child_pid == -1){
            pid_t pid = fork();
            if(pid < 0){
                perror("fork failed");
                return -1;
            } else if (pid == 0){
                //run it for time quantum using setittimer()
                raise(SIGSTOP);
                char path[512];
                snprintf(path, sizeof(path), "./%s", curr.filename);
                char *args[MAX_PARAMS + 2];
                args[0] = path;
                int k;
                for(k = 0; k < curr.nparams; k++){
                    args[k + 1] = curr.params[k];
                }
                args[k + 1] = NULL;                

                execvp(args[0], args);
                //TODO: if over time quantum, then enqueue again
                
                perror("execvp failed");
                _exit(127); 
            } else{
                int status;
                waitpid(pid, &status, WUNTRACED);
                curr.child_pid = pid;
            }
        }
        quantum_expired = 0;
        if(kill(curr.child_pid, SIGCONT) == -1){
            perror("kill(SIGCONT)");
        }
        start_timer(time_quant);
 
        int status;
        pid_t w = waitpid(curr.child_pid, &status, WUNTRACED);
 
        if(w == -1 && errno == EINTR){
            //SIGALRM interrupted us: the quantum ran out while the
            //process was still running. Preempt it.
            kill(curr.child_pid, SIGSTOP);
            waitpid(curr.child_pid, &status, WUNTRACED); //reap the stop
            //not done -> back of the line for its priority
            enqueue(&queue1, curr.priority * PRIORITY_MULT + seq++, curr);
        } else if (w == -1){
            perror("waitpid");
        } else {
            //the process changed state on its own before the quantum
            //expired, so whatever we armed is no longer needed
            cancel_timer();
 
            if(WIFEXITED(status) || WIFSIGNALED(status)){
                //finished (normally or via signal) -> out of circulation
                free(curr.filename);
                for(int k = 0; k < curr.nparams; k++){
                    free(curr.params[k]);
                }
            } else if (WIFSTOPPED(status)){
                //process suspended itself before its quantum ran out;
                //it's still alive, give it another turn later
                enqueue(&queue1, curr.priority * PRIORITY_MULT + seq++, curr);
            }
        }
    }
    
	
    return 0;

}
