#include <stdio.h>
#include <stdlib.h>
#include <SCEDA/common.h>
#include <SCEDA/queue.h>

typedef struct {
  int value;
} Integer;

Integer *new_Integer(int n) {
  Integer *x = malloc(sizeof(Integer));
  x->value = n;
  return x;
}

void delete_Integer(Integer *x) {
  free(x);
}

int main(int argc, char *argv[]) {
  // create a queue of Integer
  SCEDA_Queue *queue = SCEDA_queue_create((SCEDA_delete_fun)delete_Integer);
      
  int i;
  for(i = 0; i < 10; i++) {
    // enqueue i in the queue
    SCEDA_queue_enqueue(queue, new_Integer(i));
  }

  // empty the queue
  while(!SCEDA_queue_is_empty(queue)) {
    Integer *x;
    // dequeue next element
    SCEDA_queue_dequeue(queue, (void **)&x);
    fprintf(stdout,"%d\n",x->value);
    // do not forget to delete it
    delete_Integer(x);
  }

  // delete the queue
  SCEDA_queue_delete(queue);

  return 0;
}
