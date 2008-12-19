#include <stdio.h>
#include <stdlib.h>
#include <SCEDA/common.h>
#include <SCEDA/hashset.h>

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

int match_Integer(Integer *x, Integer *y) {
  return x->value == y->value;
}

unsigned int hash_Integer(Integer *x) {
  return x->value;
}

int main(int argc, char *argv[]) {
  // create a set of Integer
  SCEDA_HashSet *set = SCEDA_hashset_create((SCEDA_delete_fun)delete_Integer, 
					    (SCEDA_match_fun)match_Integer,
					    (SCEDA_hash_fun)hash_Integer);
      
  int i;
  for(i = 0; i < 10; i++) {
    // insert i into the set
    SCEDA_hashset_add(set, new_Integer(i));
  }

  // iterate over the set
  SCEDA_HashSetIterator elts;
  SCEDA_hashset_iterator_init(set, &elts);
  while(SCEDA_hashset_iterator_has_next(&elts)) {
    // peek the next element
    Integer *x = SCEDA_hashset_iterator_next(&elts);
    fprintf(stdout,"%d\n",x->value);
  }
  SCEDA_hashset_iterator_cleanup(&elts);

  // delete the set
  SCEDA_hashset_delete(set);

  return 0;
}
