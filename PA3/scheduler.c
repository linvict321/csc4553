#include <stdio.h>
#include "scheduler.h"
typedef struct scheduler{
    thread current;
    thread end;
    int count; //num of threads
}*scheduler;

struct scheduler rr_publish = {NULL, NULL, rr_admit, rr_remove, rr_next, rr_qlen};
scheduler RoundRobin = &rr_publish;

//put scheduler functions here,scheduler should be a circular queue?
 /* initialize any structures */
void rr_init(void){
    scheduler scheduler;
}

/* tear down any structures */
void rr_shutdown(void){

} 

/* add a thread to the pool */
void rr_admit(thread new){
    //TODO move the stack down
    scheduler.current = new;
    scheduler.count++;
    return;
} 

/* remove a thread from the pool */
void rr_remove(thread victim){

} 

/* select a thread to schedule */
thread rr_next(void){
    
} 

/* number of ready threads */
int rr_qlen(void){
    return scheduler.count;
} 
