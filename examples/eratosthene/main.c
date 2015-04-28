#include <stdio.h>
#include <stdlib.h>
#include <SCEDA/common.h>

//#include "listset.h"
#include "hashset.h"
//#include "treeset.h"

#define N 1000

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

int compare_Integer(Integer *x, Integer *y) {
  if(x->value < y->value) {
    return -1;
  } else if(x->value == y->value) {
    return 0;
  } else {
    return 1;
  }
}

unsigned int hash_Integer(Integer *x) {
  return x->value;
}

Set *eratosthene(int n) {
  Set *primes = set_create((SCEDA_delete_fun)delete_Integer,
			   (SCEDA_match_fun)match_Integer,
			   (SCEDA_compare_fun)compare_Integer,
			   (SCEDA_hash_fun)hash_Integer);

  int i;
  // first build the set [2;n]
  for(i = 2; i < n; i++) {
    set_add(primes, new_Integer(i));
  }

  // apply the sieve
  for(i = 2; i < n; i++) {
    Integer x;
    x.value = i;
    // if i is in the set
    if(set_contains(primes, &x)) {
      // remove all its multiple
      int j;
      for(j = i * i; j < n; j += i) {
	x.value = j;
	Integer *y = &x;
	if(set_remove(primes, (void **)&y) == 0) {
	  delete_Integer(y);
	}
      }
    }
  }

  return primes;
}

int main(int argc, char *argv[]) {
  // create a set of Integer

  Set *primes = eratosthene(N);

  SetIterator elts;
  set_iterator_init(primes, &elts);
  while(set_iterator_has_next(&elts)) {
    Integer *x = set_iterator_next(&elts);

    fprintf(stdout,"%d\n", x->value);
  }
  set_iterator_cleanup(&elts);

  set_delete(primes);

  return 0;
}
