#include <stdio.h>
#include <ucontext.h>
#include <sys/mman.h>

ucontext_t uc, back;

ucontext_t *running;
ucontext_t *ready;

void t_yield()
{
  ucontext_t *tmp;

  tmp = running;
  running = ready;
  ready = tmp;

  swapcontext(ready, running);
}

void assign(long a, int *b)
{
  int i;

  *b = (int)a;

  for (i = 0; i < 3; i++)
    printf("in assign(1): %d\n", i);

  t_yield();

  for (i = 10; i < 13; i++)
    printf("in assign(2): %d\n", i);

  t_yield();

  for (i = 20; i < 23; i++)
    printf("in assign(3): %d\n", i);
}

int main(int argc, char **argv) 
{
  char func_stack[16384];
  int value = 0;

  getcontext(&back);    /* let back be the context of main() */
  running = &back;

  getcontext(&uc);

  uc.uc_stack.ss_sp = func_stack;
  uc.uc_stack.ss_size = sizeof(func_stack);

  uc.uc_link = &back; 

  makecontext(&uc, assign, 2, 107L, &value);
  ready = &uc;

  printf("in main(): 0\n");

  t_yield();

  printf("in main(): 1\n");
  t_yield();

  printf("in main(): 2\n");
  t_yield();

  printf("done %d\n", value);

  return (0);
}



/* --- output -----
in main(): 0
in assign(1): 0
in assign(1): 1
in assign(1): 2
in main(): 1
in assign(2): 10
in assign(2): 11
in assign(2): 12
in main(): 2
in assign(3): 20
in assign(3): 21
in assign(3): 22
done 107
*/
