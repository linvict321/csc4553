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
//fifo queue ptrs of terminated and waiting threads
static thread wait_head = NULL;
static thread wait_tail = NULL;
static thread exit_head = NULL;
static thread exit_tail = NULL;

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

    thread t1 = (thread)malloc(sizeof(thread)); 
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
    //converts thread into lwp, creates context, adds to scheduler, don't allocate the stack
    //yield to whichever thread the scheduler chooses
    thread caller = (thread)malloc(sizeof(context));
    if(caller == NULL){
        return;
    }
    caller->tid = thread_counter++;
    caller->status = LWP_LIVE;
    curr_thread = caller;
    caller->lib_one = valid_t;
    valid_t = caller;
    my_scheduler->admit(caller);
    lwp_yield();

}

/* Yields control to another LWP, based on the scheduler choice. It saves the current LWP's context, picks
the next one, restores that thread's context and returns. If there is no next thread, terminates the
program.
It causes a thread to yield control back to the thread library, whereupon the library chooses another
thread (or maybe the same one) using the round-robin scheduler, and then runs that one.*/
void lwp_yield(void){
 //saves curr lwp context
    thread old_t = curr_thread;
    //pick next one, restore the context, returns
     //if scheduler is empty, end it otherwise return
    thread next_t = my_scheduler->next();
    if(next_t == NULL){
        exit(0);
    }

    curr_thread = next_t;
    swap_rfiles(&(old_t->state), &(next_t->state));   
}

/*Terminates the current LWP and yields to whichever thread the scheduler chooses.
 It does not return.*/
void lwp_exit(int exitval){

    //terminate current lwp, yields to thread that the scheduler chooses
    thread end_t = curr_thread;
    curr_thread->status = MKTERMSTAT(LWP_TERM, status);
    my_scheduler->remove(end_t);

    //remove from fifo queue, dealloc resources while its waiting in lwp wait
    if(wait_head != NULL){
        thread wait_pop = wait_head;
        wait_head = wait_head->lib_two; //wait head to next waiting thread?

        if(wait_head == NULL){
            wait_tail = NULL;
        }

        wait_pop->lib_two = NULL;
        wait_pop->exited = end_t;
        my_scheduler->admit(wait_pop);
    } else {
        end_t->lib_two = NULL;
        if(exit_tail != NULL){
            exit_tail->lib_two = end_t;
            exit_tail = end_t;
        } else {
            exit_head = end_t;
            exit_tail = end_t;
        }
    }

    lwp_yield();

}

/*Waits for a thread to terminate, deallocates its resources, and reports its termination status if status is
non-NULL. Returns the tid of the terminated thread or NO_THREAD*/
tid_t lwp_wait(int* status){
    
    thread terminated_thread;

    if(exit_head == NULL){ //no threads left in terminated queue
        if(scheduler->rr_qlen < 1){
            return NO_THREAD;
        }

        my_scheduler->remove(curr_thread);
        //place into thw waiting queue
        if(wait_tail == NULL){
            curr_thread->lib_two = NULL;
            wait_tail = curr_thread;
            wait_head = curr_thread;
        } else{
            curr_thread->lib_two = NULL;
            wait_tail->lib_two = curr_thread;
            wait_tail = curr_thread;
        }

        //block til thread terms
        lwp_yield();

        terminated_thread = curr_thread->exited;
        curr_thread->exited = NULL;
    } else{
        //return oldest waiting terminated exit_head
        terminated_thread = exit_head;
        exit_head = exit_head->lib_two; //next term in queue
        if(exit_head == NULL){
            exit_tail = NULL;
        }
    }
    if(status != NULL){
        *status = terminated_thread->status;
    }

    //free, remove from valid, munmap
    tid_t exit_tid = terminated_thread->tid;
    thread start = valid_t;
    thread prev = NULL;
    while(start != NULL){
        if(start->tid == terminated_thread->tid){
            break;
        }
        prev = start;
        start = start->lib_one;
    }

    //take out of valid list
    if(start != NULL){
        if(prev == NULL){
            valid_t = start->lib_one;
        } else {
            prev->lib_one = start->lib_one;
        }
    }

    if(terminated_thread->stack != NULL){ //if null, then original calling thread
        munmap(terminated_thread->stack, terminated_thread->stacksize);
    }
    free(terminated_thread);    

    return exit_tid;
}

//installing a new scheduling function
void lwp_set_scheduler(scheduler fun){
    //causes lwp to use given scheduler to choose next thread to run
    if(fun != NULL && my_scheduler != &rr_publish) {
        fun = &rr_publish;
    } else if(fun == NULL && my_scheduler == &rr_publish){
        return;
    }
    
    //transfers all threads from old scheduler to new one in next() order
    scheduler old_scheduler = my_scheduler;
    fun->init();
    while(old_scheduler->qlen() > 0){
        thread temp = old_scheduler->next();
        old_scheduler->remove(temp);
        fun->admit(temp);
    } 

    old_scheduler->shutdown();
    my_scheduler = fun;
}

//find out what the current scheduler is
scheduler lwp_get_scheduler(void){
    return my_scheduler;
}

//Returns the thread corresponding to a given thread ID, or NULL if the ID is invalid
thread tid2thread(tid_t tid){

    if(tid = NO_THREAD){
        return NULL;
    }

    thread start = valid_t;
    while(start != NULL){
        if(start->tid == tid){
            return start;
        }
        start = start->lib_one;
    }
    return NULL;
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
