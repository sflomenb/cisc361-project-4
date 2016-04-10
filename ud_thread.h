#include <stdio.h>
#include <stdlib.h>
#include <ucontext.h>
#include <sys/mman.h>

struct tcb
{
    int thread_id;
    int thread_priority;
    ucontext_t thread_context;
    struct tcb *next;
};

typedef struct tcb tcb;

void t_init(); /* Initialize the thread library by setting up the "running" and the "ready" queues and creating the "main" thread. */
void t_shutdown(); /* Shut down the thread library by freeing all the dynamically allocated memory. */
void t_create(void(*function)(int), int thread_id, int priority); /* Create a thread with priority pri, start function func with argument thr_id as the thread id. Function func should be of type void func(int). TCB of the newly created thread is added to the end of the "ready" queue; the parent thread calling t_create() continues its execution upon returning from t_create(). */
void t_terminate(); /* Terminate the calling thread by removing (and freeing) its TCB from the "running" queue, and resume execution of the thread in the head of the "ready" queue. */
void t_yield();  /* The calling thread volunarily relinquishes the CPU, and is placed at the end of the ready queue. The first thread (if there is one) in the ready queue resumes execution. */