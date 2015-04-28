#include <stdio.h>
#include <stdlib.h>
#include <SCEDA/common.h>
#include <SCEDA/list.h>

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
  // create a list of Integer
  SCEDA_List *list = SCEDA_list_create((SCEDA_delete_fun)delete_Integer);

  int i;
  for(i = 0; i < 10; i++) {
    // add i at head of the list
    SCEDA_list_add(list, new_Integer(i));
  }

  // iterate over the list
  SCEDA_ListIterator elts;
  SCEDA_list_iterator_init(list, &elts);
  while(SCEDA_list_iterator_has_next(&elts)) {
    // peek the next element
    Integer *x = SCEDA_list_iterator_next(&elts);
    fprintf(stdout,"%d\n",x->value);
  }
  SCEDA_list_iterator_cleanup(&elts);

  // delete the list
  SCEDA_list_delete(list);

  return 0;
}
