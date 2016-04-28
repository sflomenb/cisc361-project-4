#include "ud_thread.h"

tcb *running = NULL;        // Address of currently running thread
tcb *ready = NULL;          // Address of next thread to be ran in the linked list of tcb's waiting to execute
tcb *main_t = NULL;         // Address to the main thread
tcb *high_priority_ready = NULL;    // High Priority thread queue

void signal_handler(int sig){
    t_scheduler();
}

void t_scheduler(){
    signal(SIGALRM, signal_handler);
    yield();
}

// Creates a new thread and adds it to the ready queue
void t_create(void(*function)(int), int thread_id, int priority)
{
    tcb *newTcb = (tcb *) malloc(sizeof(tcb));
    newTcb->thread_id = thread_id;          // Sets the ID of the thread
    newTcb->thread_priority = priority;     // Sets the priority
    newTcb->next = NULL;                    // Explicitly sets the refernce to next to NULL


    // Max size of context stack in Hex (65536 bytes)
    size_t sz = 0x10000;
    // Pointer to new context
    ucontext_t *uc = (ucontext_t *) malloc(sizeof(ucontext_t));
    // Gets Current Context
	getcontext(uc);
	
	/*
	// Calling mmap creates a new page in memory to hold the context stack 
    uc->uc_stack.ss_sp = mmap(0, sz,
       PROT_READ | PROT_WRITE | PROT_EXEC,
       MAP_PRIVATE | MAP_STACK | MAP_ANON, -1, 0);
    */
	
	// Context stack setup
	uc->uc_stack.ss_sp = malloc(sz);  /* new statement */
	uc->uc_stack.ss_size = sz;
	uc->uc_stack.ss_flags = 0;
	// Sets the return context
	uc->uc_link = main_t->thread_context;
	// Makes the new context
	makecontext(uc, (void (*)(void))function, 1, thread_id);
	// Sets a reference to the context in the tcb
	newTcb->thread_context = uc;
	
    
    
    
    // Appends the new tcb to the end of the linked list queue.
    // Checks first to see if the list is empty
    if(ready == NULL)
    {
        // Sets ready to point to the memory address of newTcb
        ready = newTcb;
    }
    else
    {
        // The focus pointer points to the *address* of the tcb we will update
        tcb *focus = ready;
        // Walk the list until we find that the current tcb does not have a next element
        while(focus->next != NULL)
        {
            focus = (tcb*)(focus->next);
        }
        // Stes the next element of the last element in the list to the newTcb
        focus->next = (tcb*)newTcb;
    }

   
}

// Called by a thread to stop executiona and give processing power to the next thread in the queue
void t_yield()
{
    // Stops any alarms that have been set
    ualarm(0, 0);
    // Checks to see if there is another thread to take its place
    if(ready == NULL)
    {
        // If not then this function returns because there is no other thread to
        // yield to.
        return;
    }
    
    // Saves current context
    tcb *oldThread = running;
        
    // The focus pointer points to the *address* of the tcb we will update
    tcb *focus = ready;
        
    // Walk the list until we find that the current tcb does not have a next element
    while(focus->next != NULL) 
    {
        focus = focus->next;
    }
        
    // Adds the current running tcb to the back of the queue
    focus->next = oldThread;
        
    // Sets the current running pointer to point to the address of the 1st thread
    // in the ready queue
    running = ready;
    // Sets the front of the queue to point to address of the 2nd thread waiting
    // to execute
    ready = ready->next;
    // Sets the the thread pointed to by running to not point to the top of the queue
    running->next = NULL;
    
    // Sets timer prior to context switching
    ualarm(1, 0);
    // Swaps running swaps context with the old thread.
    swapcontext(oldThread->thread_context, running->thread_context);
    
}

// Initializes the thread library
void t_init()
{
  // Creates a tcb for the main thread
  main_t = (tcb *) malloc(sizeof(tcb));
  main_t->thread_id = 0;
  main_t->thread_priority = 1;
  main_t->next = NULL;
  ucontext_t *uc = (ucontext_t *) malloc(sizeof(ucontext_t));
  getcontext(uc);
  main_t->thread_context = uc;

  running = main_t;
  ready = NULL;
  
  signal(SIGALRM, signal_handler);
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
    running->next = NULL;

    free(tmp->thread_context->uc_stack.ss_sp);
    //munmap(tmp->thread_context->uc_stack.ss_sp,tmp->thread_context->uc_stack.ss_size);
    free(tmp->thread_context);
    free(tmp);
    
    // Sets the new context
    if(running) //  != NULL
    {
        setcontext(running->thread_context);
    }
}

// Called to free all library resources
void t_shutdown()
{
    free(main_t->thread_context->uc_stack.ss_sp);
    free(main_t->thread_context);
    free(main_t);
    tcb *focus = ready;
    tcb *tmp;
    while (focus != NULL) {
        tmp = focus;
        focus = focus->next;
        free(tmp->thread_context->uc_stack.ss_sp);
        free(tmp->thread_context);
        free(tmp);
    }
}
