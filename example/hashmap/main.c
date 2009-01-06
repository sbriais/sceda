#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <SCEDA/common.h>
#include <SCEDA/hashmap.h>

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

unsigned int hash_string(const char *ptr) {
  int val = 0;

  while(*ptr != '\0') {
    int tmp;
    val = (val << 4) + (*ptr);
    if((tmp = (val & 0xf0000000))) {
      val = val ^ (tmp >> 24);
      val = val ^ tmp;
    }
    ptr++;
  }

  return (unsigned int)val;
}

int match_string(const char *s1, const char *s2) {
  return (strcmp(s1, s2) == 0);
}

void delete_string(char *s) {
  free(s);
}

int main(int argc, char *argv[]) {
  // create a set of Integer
  SCEDA_HashMap *map = SCEDA_hashmap_create((SCEDA_delete_fun)delete_string, 
					    (SCEDA_delete_fun)delete_Integer,
					    (SCEDA_match_fun)match_string,
					    (SCEDA_hash_fun)hash_string);
  
  // note that string are allocated onto the heap, thanks to strdup function
  SCEDA_hashmap_put(map, strdup("Odissey"), new_Integer(2001), NULL);
  SCEDA_hashmap_put(map, strdup("Torment"), new_Integer(2002), NULL);
  SCEDA_hashmap_put(map, strdup("Judgment"), new_Integer(2002), NULL);
  SCEDA_hashmap_put(map, strdup("Onslaught"), new_Integer(2002), NULL);
  SCEDA_hashmap_put(map, strdup("Legions"), new_Integer(2003), NULL);
  SCEDA_hashmap_put(map, strdup("Scourge"), new_Integer(2003), NULL);
  SCEDA_hashmap_put(map, strdup("Mirrodin"), new_Integer(2003), NULL);
  SCEDA_hashmap_put(map, strdup("Darksteel"), new_Integer(2004), NULL);
  SCEDA_hashmap_put(map, strdup("Fifth Dawn"), new_Integer(2004), NULL);
  SCEDA_hashmap_put(map, strdup("Champions of Kamigawa"), new_Integer(2004), NULL);

  // iterate over the map
  SCEDA_HashMapIterator elts;
  SCEDA_hashmap_iterator_init(map, &elts);
  while(SCEDA_hashmap_iterator_has_next(&elts)) {
    // peek the next element
    char *key;
    Integer *x = SCEDA_hashmap_iterator_next(&elts, &key);
    fprintf(stdout,"%s -> %d\n",key, x->value);
  }
  SCEDA_hashmap_iterator_cleanup(&elts);

  // delete the set
  SCEDA_hashmap_delete(map);

  return 0;
}
