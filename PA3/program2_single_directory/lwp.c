#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/resource.h>
#include <sys/mman.h>
#include "rr.h"
#include "lwp.h"
#include "fp.h"

static int thread_counter = 1;
static scheduler my_scheduler = &rr_publish;
static thread curr_thread = NULL;
static thread valid_t = NULL; //points to head of linked list of threads

static void lwp_wrap(lwpfun, void);
tid_t lwp_create(lwpfun,void *);
void  lwp_exit(int status);
tid_t lwp_gettid(void);
void  lwp_yield(void);
void  lwp_start(void);
tid_t lwp_wait(int *);
void  lwp_set_scheduler(scheduler fun);
scheduler lwp_get_scheduler(void);
thread tid2thread(tid_t tid);

/*Creates a new lightweight process (thread) which executes the given function with the given
argument. It returns the lightweight thread id of the new thread or NO_THREAD if the thread cannot be
created */
tid_t lwp_create(lwpfun function, void* argument){

    struct rlimit limit;
    size_t stack_size;  //initialize stack; malloc won't care if it overflows, we want to use 'mmap'

    thread t1 = (*thread)malloc(sizeof(thread)); 
    if(t1 == NULL){ //if thread fails
        return NO_THREAD;
    }

    /*The threadʼs resources will consist of a context and stack, 
    both initialized so that when the scheduler chooses this thread and its
    context is loaded via swap_rfiles it will run the given function*/
    thread->tid = thread_counter++;
    long page_size = sysconf(_SC_PAGE_SIZE); //stacks must be a multiple of memory page size
    stack_size = 8*1024*1024; //8MB
    //check rlimit
    if(getrlimit(RLIMIT_STACK, &limit) == 0 || limit.rlim_cur != RLIM_INFINITY){
        //choose 8MB
        stack_size = (size_t)limit.rlim_cur; 
    }
    //if resource limit is not a multiple of page size, if not, round up
    if(limit.rlim_cur % page_size != 0){
        long next_mult = (stack_size + page_size - 1)/page_size;
        t1->stacksize = next_mult * page_size;
    }
    //intialize thread pointer
    t1->stack = mmap(NULL, stack_size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS | MAP_STACK, -1, 0);
    if(t1->stack == MAP_FAILED){
        free(t1);
        return NO_THREAD;
    }
    
    t1->status = LWP_LIVE;

    //set up the stack pointers
    unsigned long *end_stack = t1->stack + (stack_size / sizeof(unsigned long)); //stack is word addressable, convert stack_size to word
    end_stack[-3] = 0; //fake saved rbp, popped so rsp goes to next slot
    end_stack[-2] = (unsigned long)lwp_wrap; //return addr, ret pops this in rip (where thread starts)
    end_stack[-1] = 0; //placeholder, occupies spot so registers come out right

    
    //saves a state
    t1->state.fxsave = FPU_INIT;
    t1->stack.rbp = (unsigned long)end_stack;
    t1->stack.rsp = (unsigned long)end_stack;
    t1->stack.rdi = (unsigned long)function;
    t1->stack.rsi = (unsigned long)argument;
    // thread will execute until it either calls lwp_exit() or the function returns with a termination status
    
    //add to lib list of valid threads and admit to curr scheduler
    t1->lib_one = valid_t; //new thread points to head
    valid_t = t1; //new thread becomes the head
    my_scheduler->admit(t1);

    return t1->tid;
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
//unmap
    munmap(2);
}

/*Waits for a thread to terminate, deallocates its resources, and reports its termination status if status is
non-NULL. Returns the tid of the terminated thread or NO_THREAD*/
tid_t lwp_wait(int* status){
    
    if(scheduler->rr_qlen <= 1){
        return NO_THREAD;
    }

    if(status){
        /* threadʼs status consists of a flag indicating whether it is running (LWP_LIVE) or terminated (LWP_TERM)
        and an 8-bit integer that can be passed back via lwp_wait(). A threadʼs termination value is the low 8
        bits either of the argument to lwp_exit() or of the return value of the thread function. These are
        combined into a single integer using the macro MKTERMSTAT() which is what is passed back by
        lwp_wait(). Macros for dealing with termination statuses shown in table below*/

    }

    return thread.id
}

//installing a new scheduling function
void lwp_set_scheduler(scheduler fun){

}

//find out what the current scheduler is
scheduler lwp_get_scheduler(void){

}

//Returns the thread corresponding to a given thread ID, or NULL if the ID is invalid
thread tid2thread(tid_t tid){

    if(!tid){
        return NULL;
    }

    return tid->tid_t;
}

//Returns the tid of the calling LWP or NO_THREAD if not called by an LWP.
tid_t lwp_gettid(void){

    if(curr_thread == NULL){
        return NO_THREAD;
    }

    return curr_thread->tid;    
}

static void lwp_wrap(lwpfun fun, void* arg) {
    /*
    * Call the given function with the given argument
    * Call lwp_exit() with its return value
    */
    int rval;
    rval = fun(arg);
    lwp_exit(rval);
}
