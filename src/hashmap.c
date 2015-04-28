/*
   Copyright Sebastien Briais 2008, 2009

   This file is part of SCEDA.

   SCEDA is free software: you can redistribute it and/or modify it
   under the terms of the GNU Lesser General Public License as
   published by the Free Software Foundation, either version 3 of the
   License, or (at your option) any later version.

   SCEDA is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with SCEDA.  If not, see
   <http://www.gnu.org/licenses/>.
*/
#include "hashmap.h"
#include "util.h"
#include <string.h>

#define new_hash_code(hmap, key, buckets) (((hmap)->hash_key(key)) % buckets)
#define hash_code(hmap, key) (new_hash_code(hmap, key, (hmap)->buckets))

void SCEDA_hashmap_init(SCEDA_HashMap *hmap, SCEDA_delete_fun delete_key, SCEDA_delete_fun delete_value,
		  SCEDA_match_fun match_key, SCEDA_hash_fun hash_key) {
  hmap->buckets = SCEDA_DFLT_SIZE;
  hmap->size = 0;
  hmap->delete_key = delete_key;
  hmap->delete_value = delete_value;
  hmap->match_key = match_key;
  hmap->hash_key = hash_key;

  hmap->table = safe_calloc(hmap->buckets, sizeof(SCEDA_ListMap));
  int i;
  for(i = 0; i < hmap->buckets; i++) {
    SCEDA_listmap_init(SCEDA_hashmap_nth_map(hmap, i), delete_key, delete_value, match_key);
  }
}

SCEDA_HashMap *SCEDA_hashmap_create(SCEDA_delete_fun delete_key, SCEDA_delete_fun delete_value,
			SCEDA_match_fun match_key, SCEDA_hash_fun hash_key) {
  SCEDA_HashMap *hmap = (SCEDA_HashMap *)safe_malloc(sizeof(SCEDA_HashMap));
  SCEDA_hashmap_init(hmap, delete_key, delete_value, match_key, hash_key);
  return hmap;
}

void SCEDA_hashmap_cleanup(SCEDA_HashMap *hmap) {
  int i;
  for(i = 0; i < hmap->buckets; i++) {
    SCEDA_listmap_cleanup(SCEDA_hashmap_nth_map(hmap, i));
  }
  free(hmap->table);

  memset(hmap, 0, sizeof(SCEDA_HashMap));
}

void SCEDA_hashmap_delete(SCEDA_HashMap *hmap) {
  SCEDA_hashmap_cleanup(hmap);
  free(hmap);
}

void SCEDA_hashmap_clear(SCEDA_HashMap *hmap) {
  SCEDA_delete_fun delete_key = hmap->delete_key;
  SCEDA_delete_fun delete_value = hmap->delete_value;
  SCEDA_match_fun match_key = hmap->match_key;
  SCEDA_hash_fun hash_key = hmap->hash_key;
  SCEDA_hashmap_cleanup(hmap);
  SCEDA_hashmap_init(hmap, delete_key, delete_value, match_key, hash_key);
}

#define nth_map(table, i) (&((*table)[i]))

static void SCEDA_hashmap_resize(SCEDA_HashMap *hmap, int buckets) {
  if((buckets <= 1) || (buckets == hmap->buckets)) {
    return;
  }

  SCEDA_ListMap (*new_table)[] = safe_calloc(buckets, sizeof(SCEDA_ListMap));
  int i;
  for(i = 0; i < buckets; i++) {
    SCEDA_listmap_init(nth_map(new_table, i), hmap->delete_key,
		 hmap->delete_value, hmap->match_key);
  }

  for(i = 0; i < hmap->buckets; i++) {
    SCEDA_ListMap *map = SCEDA_hashmap_nth_map(hmap, i);
    while(SCEDA_listmap_size(map) > 0) {
      void *key;
      void *value;
      SCEDA_listmap_remove_head(map, &key, &value);
      int j = new_hash_code(hmap, key, buckets);
      SCEDA_listmap_add_tail(nth_map(new_table, j), key, value);
    }
    SCEDA_listmap_cleanup(map);
  }

  free(hmap->table);
  hmap->table = new_table;
  hmap->buckets = buckets;
}

int SCEDA_hashmap_put(SCEDA_HashMap *hmap, const void *key,
		const void *value, void **old_value) {
  int i = hash_code(hmap, key);

  int res = SCEDA_listmap_put(SCEDA_hashmap_nth_map(hmap, i), key, value, old_value);

  if(res == 0) {
    hmap->size++;
    if(hmap->size > hmap->buckets) {
      SCEDA_hashmap_resize(hmap, 1 + 2 * hmap->buckets);
    }
  }

  return res;
}

int SCEDA_hashmap_remove(SCEDA_HashMap *hmap, void **key, void **value) {
  int i = hash_code(hmap, *key);

  int res = SCEDA_listmap_remove(SCEDA_hashmap_nth_map(hmap, i), key, value);

  if(res == 0) {
    hmap->size--;
    if(4 * hmap->size < hmap->buckets) {
      SCEDA_hashmap_resize(hmap, 1 + hmap->buckets / 2);
    }
  }

  return res;
}

int SCEDA_hashmap_contains_key(SCEDA_HashMap *hmap, const void *key) {
  int i = hash_code(hmap, key);

  return SCEDA_listmap_contains_key(SCEDA_hashmap_nth_map(hmap, i), key);
}

void *SCEDA_hashmap_get(SCEDA_HashMap *hmap, const void *key) {
  int i = hash_code(hmap, key);

  return SCEDA_listmap_get(SCEDA_hashmap_nth_map(hmap, i), key);
}

int SCEDA_hashmap_lookup(SCEDA_HashMap *hmap, void **key, void **value) {
  int i = hash_code(hmap, *key);

  return SCEDA_listmap_lookup(SCEDA_hashmap_nth_map(hmap, i), key, value);
}
