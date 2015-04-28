#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <SCEDA/common.h>
#include <SCEDA/listmap.h>

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

int match_string(const char *s1, const char *s2) {
  return (strcmp(s1, s2) == 0);
}

void delete_string(char *s) {
  free(s);
}

int main(int argc, char *argv[]) {
  // create a set of Integer
  SCEDA_ListMap *map = SCEDA_listmap_create((SCEDA_delete_fun)delete_string,
					    (SCEDA_delete_fun)delete_Integer,
					    (SCEDA_match_fun)match_string);

  // note that strings are allocated onto the heap, thanks to strdup function
  SCEDA_listmap_put(map, strdup("Odissey"), new_Integer(2001), NULL);
  SCEDA_listmap_put(map, strdup("Torment"), new_Integer(2002), NULL);
  SCEDA_listmap_put(map, strdup("Judgment"), new_Integer(2002), NULL);
  SCEDA_listmap_put(map, strdup("Onslaught"), new_Integer(2002), NULL);
  SCEDA_listmap_put(map, strdup("Legions"), new_Integer(2003), NULL);
  SCEDA_listmap_put(map, strdup("Scourge"), new_Integer(2003), NULL);
  SCEDA_listmap_put(map, strdup("Mirrodin"), new_Integer(2003), NULL);
  SCEDA_listmap_put(map, strdup("Darksteel"), new_Integer(2004), NULL);
  SCEDA_listmap_put(map, strdup("Fifth Dawn"), new_Integer(2004), NULL);
  SCEDA_listmap_put(map, strdup("Champions of Kamigawa"), new_Integer(2004), NULL);

  // iterate over the map
  SCEDA_ListMapIterator elts;
  SCEDA_listmap_iterator_init(map, &elts);
  while(SCEDA_listmap_iterator_has_next(&elts)) {
    // peek the next element
    char *key;
    Integer *x = SCEDA_listmap_iterator_next(&elts, &key);
    fprintf(stdout,"%s -> %d\n",key, x->value);
  }
  SCEDA_listmap_iterator_cleanup(&elts);

  // delete the set
  SCEDA_listmap_delete(map);

  return 0;
}
