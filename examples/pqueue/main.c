#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <SCEDA/common.h>
#include <SCEDA/pqueue.h>

typedef struct {
  char *name;
  int priority;
} Task;

Task *new_Task(const char *name, int prio) {
  Task *x = malloc(sizeof(Task));
  x->name = strdup(name);
  x->priority = prio;
  return x;
}

void delete_Task(Task *x) {
  free(x->name);
  free(x);
}

int compare_Task(Task *x, Task *y) {
  if(x->priority < y->priority) {
    return -1;
  } else if(x->priority == y->priority) {
    return 0;
  } else {
    return 1;
  }
}

int main(int argc, char *argv[]) {
  // create a priority queue
  SCEDA_PQueue *pqueue = SCEDA_pqueue_create((SCEDA_delete_fun)delete_Task, (SCEDA_compare_fun)compare_Task);

  SCEDA_pqueue_insert(pqueue, new_Task("free", 4));
  SCEDA_pqueue_insert(pqueue, new_Task("cleanup", 3));
  SCEDA_pqueue_insert(pqueue, new_Task("alloc",1));
  SCEDA_pqueue_insert(pqueue, new_Task("init", 2));

  while(!SCEDA_pqueue_is_empty(pqueue)) {
    Task *t;
    SCEDA_pqueue_extract(pqueue, (void **)&t);
    fprintf(stdout,"%s of priority %d\n", t->name, t->priority);
    delete_Task(t);
  }

  // delete the priority queue
  SCEDA_pqueue_delete(pqueue);

  return 0;
}
