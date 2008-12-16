/*
   This file is part of SCEDA.
   (c) 2008 Sebastien Briais
   
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
#include "hashset.h"
#include "util.h"
#include <string.h>

#define new_hash_code(hset, data, buckets) (((hset)->hash(data)) % buckets)
#define hash_code(hset, data) (new_hash_code(hset, data, (hset)->buckets))

void SCEDA_hashset_init(SCEDA_HashSet *hset, SCEDA_delete_fun delete, 
		  SCEDA_match_fun match, SCEDA_hash_fun hash) {
  hset->buckets = DFLT_SIZE;
  hset->size = 0;
  hset->delete = delete;
  hset->match = match;
  hset->hash = hash;

  hset->table = safe_calloc(hset->buckets, sizeof(SCEDA_ListSet));
  int i;
  for(i = 0; i < hset->buckets; i++) {
    SCEDA_listset_init(SCEDA_hashset_nth_set(hset, i), delete, match);
  }
}

SCEDA_HashSet *SCEDA_hashset_create(SCEDA_delete_fun delete, SCEDA_match_fun match,
			SCEDA_hash_fun hash) {
  SCEDA_HashSet *hset = (SCEDA_HashSet *)safe_malloc(sizeof(SCEDA_HashSet));
  SCEDA_hashset_init(hset, delete, match, hash);
  return hset;
}

void SCEDA_hashset_cleanup(SCEDA_HashSet *hset) {
  int i;
  for(i = 0; i < hset->buckets; i++) {
    SCEDA_listset_cleanup(SCEDA_hashset_nth_set(hset, i));
  }
  free(hset->table);

  memset(hset, 0, sizeof(SCEDA_HashSet));
}

void SCEDA_hashset_delete(SCEDA_HashSet *hset) {
  SCEDA_hashset_cleanup(hset);
  free(hset);
}

void SCEDA_hashset_clear(SCEDA_HashSet *hset) {
  SCEDA_delete_fun delete = hset->delete;
  SCEDA_match_fun match = hset->match;
  SCEDA_hash_fun hash = hset->hash;
  SCEDA_hashset_cleanup(hset);
  SCEDA_hashset_init(hset, delete, match, hash);
}

#define nth_set(table, i) (&((*table)[i]))

static void SCEDA_hashset_resize(SCEDA_HashSet *hset, int buckets) {
  if((buckets <= 1) || (buckets == hset->buckets)) {
    return;
  }

  SCEDA_ListSet (*new_table)[] = safe_calloc(buckets, sizeof(SCEDA_ListSet));
  int i;
  for(i = 0; i < buckets; i++) {
    SCEDA_listset_init(nth_set(new_table, i), hset->delete, hset->match);
  }

  for(i = 0; i < hset->buckets; i++) {
    SCEDA_ListSet *set = SCEDA_hashset_nth_set(hset, i);
    while(SCEDA_listset_size(set) > 0) {
      void *data;
      SCEDA_listset_remove_head(set, &data);
      int j = new_hash_code(hset, data, buckets);
      SCEDA_listset_add_tail(nth_set(new_table, j), data);
    }
    SCEDA_listset_cleanup(set);
  }

  free(hset->table);
  hset->table = new_table;
  hset->buckets = buckets;
}

int SCEDA_hashset_add(SCEDA_HashSet *hset, const void *data) {
  int i = hash_code(hset, data);
  
  int res = SCEDA_listset_add(SCEDA_hashset_nth_set(hset, i), data);

  if(res == 0) {
    hset->size++;
    if(hset->size > hset->buckets) {
      SCEDA_hashset_resize(hset, 1 + 2 * hset->buckets);
    }
  } 

  return res;
}

int SCEDA_hashset_remove(SCEDA_HashSet *hset, void **data) {
  int i = hash_code(hset, *data);
  
  int res = SCEDA_listset_remove(SCEDA_hashset_nth_set(hset, i), data);
  
  if(res == 0) {
    hset->size--;
    if(4 * hset->size < hset->buckets) {
      SCEDA_hashset_resize(hset, 1 + hset->buckets / 2);
    }
  }

  return res;
}

int SCEDA_hashset_contains(SCEDA_HashSet *hset, const void *data) {
  int i = hash_code(hset, data);

  return SCEDA_listset_contains(SCEDA_hashset_nth_set(hset, i), data);
}

int SCEDA_hashset_lookup(SCEDA_HashSet *hset, void **data) {
  int i = hash_code(hset, *data);

  return SCEDA_listset_lookup(SCEDA_hashset_nth_set(hset, i), data);
}

