#include <stdio.h>
#include <stdlib.h>
#include <SCEDA/common.h>
#include <SCEDA/stack.h>

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
  // create a stack of Integer
  SCEDA_Stack *stack = SCEDA_stack_create((SCEDA_delete_fun)delete_Integer);
      
  int i;
  for(i = 0; i < 10; i++) {
    // push i onto the stack
    SCEDA_stack_push(stack, new_Integer(i));
  }

  // empty the stack
  while(!SCEDA_stack_is_empty(stack)) {
    Integer *x;
    // pop next element
    SCEDA_stack_pop(stack, (void **)&x);
    fprintf(stdout,"%d\n",x->value);
    // do not forget to delete it
    delete_Integer(x);
  }

  // delete the stack
  SCEDA_stack_delete(stack);

  return 0;
}
