#include <stdio.h>

#include "lwp.h"

int thread_counter = 1;
static scheduler my_scheduler = NULL;

/*Creates a new lightweight process (thread) which executes the given function with the given
argument. It returns the lightweight thread id of the new thread or NO_THREAD if the thread cannot be
created */
tid_t lwp_create(lwpfun function, void* argument){

    thread t1; 
    if(t1 == NULL){ //if thread fails
        return NO_THREAD;
    }


    /*The threadʼs resources will consist of a context and stack, 
    both initialized so that when the scheduler chooses this thread and its
    context is loaded via swap_rfiles it will run the given function*/
    context c1 = swap_rfiles(); 
    c1.id = thread_counter;
    thread_counter++;

    //initialize stack
    
    my_scheduler->admit(t1);

    // thread will execute until it either calls lwp_exit() or the function returns with a termination status
    

    return thread.id;
}

//start the lwp, converts calling thread into lwp
void lwp_start(void){
    //converts thread into lwp
    //yield to whichever thread the scheduler chooses
    
}

/* Yields control to another LWP, based on the scheduler choice. It saves the current LWP's context, picks
the next one, restores that thread's context and returns. If there is no next thread, terminates the
program.
It causes a thread to yield control back to the thread library, whereupon the library chooses another
thread (or maybe the same one) using the round-robin scheduler, and then runs that one.*/
void lwp_yield(void){

}

/*Terminates the current LWP and yields to whichever thread the scheduler chooses.
 It does not return.*/
void lwp_exit(int exitval){

    // dealloc stack
    //malloc won't care if it overflows, we want to use 'mmap'
}

/*Waits for a thread to terminate, deallocates its resources, and reports its termination status if status is
non-NULL. Returns the tid of the terminated thread or NO_THREAD*/
tid_t lwp_wait(int* status){

    if(){
        return NO_THREAD;
    }

    return thread.id
}

void lwp_set_scheduler(scheduler fun){

}
scheduler lwp_get_scheduler(void){

}
thread tid2thread(tid_t tid){

}
