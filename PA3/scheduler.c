#include <stdio.h>
#include "scheduler.h"

//put scheduler functions here,scheduler should be a circular queue?
void s_init(void){
    

} /* initialize any structures */
void s_shutdown(void){

} /* tear down any structures */
void s_admit(thread new){

} /* add a thread to the pool */
void s_remove(thread victim){

} /* remove a thread from the pool */
thread s_next(void){

} /* select a thread to schedule */
int s_qlen(void){

} /* number of ready threads */
