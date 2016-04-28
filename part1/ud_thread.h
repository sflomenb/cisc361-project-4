#include <stdio.h>
#include <stdlib.h>
#include <sys/ucontext.h>
#include <stdio.h>
#include <sys/types.h>
#include <err.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <ucontext.h>
#include <stdbool.h>
#include <sys/mman.h>

typedef struct tcb tcb;

struct tcb
{
    int thread_id;
    int thread_priority;
    ucontext_t *thread_context;
    tcb *next;
};


void t_init(); /* Initialize the thread library by setting up the "running" and the "ready" queues and creating the "main" thread. */
void t_shutdown(); /* Shut down the thread library by freeing all the dynamically allocated memory. */
void t_create(void(*function)(int), int thread_id, int priority); /* Create a thread with priority pri, start function func with argument thr_id as the thread id. Function func should be of type void func(int). TCB of the newly created thread is added to the end of the "ready" queue; the parent thread calling t_create() continues its execution upon returning from t_create(). */
void t_terminate(); /* Terminate the calling thread by removing (and freeing) its TCB from the "running" queue, and resume execution of the thread in the head of the "ready" queue. */
void t_yield();  /* The calling thread volunarily relinquishes the CPU, and is placed at the end of the ready queue. The first thread (if there is one) in the ready queue resumes execution. */
void signal_handler(int sig); //Handles the SIGALRM signals from the time slicing
void t_scheduler(); //Scheduler that handles the context switching of the threads