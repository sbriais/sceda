#include <stdio.h>
#include <stdlib.h>
#include <SCEDA/common.h>
#include <SCEDA/treeset.h>

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
  // create a set of Integer
  SCEDA_TreeSet *set = SCEDA_treeset_create((SCEDA_delete_fun)delete_Integer,
					    (SCEDA_compare_fun)compare_Integer);

  int i;
  for(i = 0; i < 10; i++) {
    // insert i into the set
    SCEDA_treeset_add(set, new_Integer(i));
  }

  // iterate over the set
  SCEDA_TreeSetIterator elts;
  SCEDA_treeset_iterator_init(set, &elts);
  while(SCEDA_treeset_iterator_has_next(&elts)) {
    // peek the next element
    Integer *x = SCEDA_treeset_iterator_next(&elts);
    fprintf(stdout,"%d\n",x->value);
  }
  SCEDA_treeset_iterator_cleanup(&elts);

  // delete the set
  SCEDA_treeset_delete(set);

  return 0;
}
