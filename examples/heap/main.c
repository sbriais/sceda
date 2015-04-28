#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <SCEDA/common.h>
#include <SCEDA/heap.h>

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

void delete_string(char *s) {
  free(s);
}

int compare_Integer(Integer *x, Integer *y) {
  if(x->value < y->value) {
    return -1;
  } else if(x->value == y->value) {
    return 0;
  } else {
    return 1;
  }
}

int main(int argc, char *argv[]) {
  // create a priority queue
  SCEDA_Heap *heap = SCEDA_heap_create((SCEDA_delete_fun)delete_string, (SCEDA_delete_fun)delete_Integer,(SCEDA_compare_fun)compare_Integer);

  SCEDA_heap_insert(heap, strdup("free"), new_Integer(4));
  SCEDA_heap_insert(heap, strdup("cleanup"), new_Integer(3));
  SCEDA_heap_insert(heap, strdup("alloc"), new_Integer(1));
  SCEDA_heap_insert(heap, strdup("init"), new_Integer(2));

  while(!SCEDA_heap_is_empty(heap)) {
    char *name;
    Integer *priority;
    SCEDA_heap_extract(heap, (void **)&name, (void **)&priority);
    fprintf(stdout,"%s of priority %d\n", name, priority->value);
    delete_string(name);
    delete_Integer(priority);
  }

  // delete the priority queue
  SCEDA_heap_delete(heap);

  return 0;
}
