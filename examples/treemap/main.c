#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <SCEDA/common.h>
#include <SCEDA/treemap.h>

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

int compare_string(const char *s1, const char *s2) {
  return strcmp(s1, s2);
}

void delete_string(char *s) {
  free(s);
}

int main(int argc, char *argv[]) {
  // create a set of Integer
  SCEDA_TreeMap *map = SCEDA_treemap_create((SCEDA_delete_fun)delete_string,
					    (SCEDA_delete_fun)delete_Integer,
					    (SCEDA_compare_fun)compare_string);

  // note that strings are allocated onto the heap, thanks to strdup function
  SCEDA_treemap_put(map, strdup("Odissey"), new_Integer(2001), NULL);
  SCEDA_treemap_put(map, strdup("Torment"), new_Integer(2002), NULL);
  SCEDA_treemap_put(map, strdup("Judgment"), new_Integer(2002), NULL);
  SCEDA_treemap_put(map, strdup("Onslaught"), new_Integer(2002), NULL);
  SCEDA_treemap_put(map, strdup("Legions"), new_Integer(2003), NULL);
  SCEDA_treemap_put(map, strdup("Scourge"), new_Integer(2003), NULL);
  SCEDA_treemap_put(map, strdup("Mirrodin"), new_Integer(2003), NULL);
  SCEDA_treemap_put(map, strdup("Darksteel"), new_Integer(2004), NULL);
  SCEDA_treemap_put(map, strdup("Fifth Dawn"), new_Integer(2004), NULL);
  SCEDA_treemap_put(map, strdup("Champions of Kamigawa"), new_Integer(2004), NULL);

  // iterate over the map
  SCEDA_TreeMapIterator elts;
  SCEDA_treemap_iterator_init(map, &elts);
  while(SCEDA_treemap_iterator_has_next(&elts)) {
    // peek the next element
    char *key;
    Integer *x = SCEDA_treemap_iterator_next(&elts, (void **)&key);
    fprintf(stdout,"%s -> %d\n",key, x->value);
  }
  SCEDA_treemap_iterator_cleanup(&elts);

  // delete the set
 SCEDA_treemap_delete(map);

  return 0;
}
