#include "ud_thread.h"

tcb *running;       // Currently running thread
tcb *ready;         // Linked list of tcb's waiting to execute
tcb *main_t;          // Reference back to the main thread (This will most likely be deleted)

// Creates a new thread and adds it to the ready queue
void t_create(void(*function)(int), int thread_id, int priority)
{
    tcb *newTcb = malloc(sizeof(tcb));  // our new tcb which will go at the end
    newTcb->thread_id = thread_id;
    newTcb->thread_priority = priority;
    
    // original code **********************
    size_t sz = 0x10000;

    ucontext_t *uc;
    uc = (ucontext_t *) malloc(sizeof(ucontext_t));
    newTcb->thread_context = *uc;
    
    getcontext(uc);
    /***
    uc->uc_stack.ss_sp = mmap(0, sz,
       PROT_READ | PROT_WRITE | PROT_EXEC,
       MAP_PRIVATE | MAP_ANON, -1, 0);
    ***/
    uc->uc_stack.ss_sp = malloc(sz);  /* new statement */
    uc->uc_stack.ss_size = sz;
    uc->uc_stack.ss_flags = 0;
    uc->uc_link = &(main_t->thread_context); 
    // original code *********************
    
    tcb *focus = ready;  // tcb pointing to previous
    if(ready == NULL) {
        ready = newTcb;
    }else{
    
        while(focus->next != NULL) {
            focus = focus->next;
        }
        focus->next = newTcb;
        focus->thread_context.uc_link = &(main_t->thread_context);
    }
    
    makecontext(uc, function, 1, thread_id);
}

// Called by a thread to stop executiona and give processing power to the next thread in the queue
void t_yield()
{
    // Save current context to a temp variable
    tcb *tmp = running;
    
    // Add the current tcb to the ready queue
    tcb *focus = ready;
    if(ready == NULL){
        // Sets ready equal to temp if the queue is empty
        ready = tmp;
    }else{
        // Loops through the queue
        while(focus->next != NULL){
            focus = focus->next;
        }
        // Adds the current running tcb to the back of the queue
        focus->next = tmp;
    }
    
    running = ready;
    running->next = NULL;
    ready = ready->next;
    swapcontext(&(running->thread_context),&(tmp->thread_context));

}

// Initializes the thread library
void t_init()
{
    // Creates a tcb for the main thread
    tcb *tmp = (tcb *) malloc(sizeof(tcb));
    getcontext(&(tmp->thread_context));
    tmp->thread_id = 0;
    tmp->thread_priority = 1;
    running = tmp;
    main_t = running;
}

// Called by a thread to stop its execution and free its tcb reference
void t_terminate()
{
    // Temp pointer is set to current running tcb
    tcb *tmp = running;
    // Set the running pointer equal to the tcb at the font of the ready queue
    running = ready;
    // Set the head of the ready queue to the next item in the list
    ready = ready->next;
    // Free resources associated with tmp
    free(&(tmp->thread_context.uc_stack.ss_sp));
    free(&(tmp->thread_context));
    free(tmp);
    // Sets the new context
    setcontext(&(running->thread_context));
}

// Called to free all library resources
void t_shutdown()
{
    // uc strack.ss.sp
    // context 
    // tcb
    
    tcb *focus = ready;
    tcb *temp = focus;
    while (focus != NULL) {
        temp = focus;
        free(focus->thread_context.uc_stack.ss_sp);
        free(&(focus->thread_context));
        focus = focus->next;
        free(temp);
    }
}


