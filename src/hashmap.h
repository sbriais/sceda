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
#ifndef SCEDA_HASHMAP_H
#define SCEDA_HASHMAP_H
/** \file hashmap.h
    \brief HashMap implementation */

#include "common.h"
#include "listmap.h"
#include <string.h>

typedef struct {
  int buckets;
  int size;
  SCEDA_delete_fun delete_key;
  SCEDA_delete_fun delete_value;
  SCEDA_match_fun match_key;
  SCEDA_hash_fun hash_key;
  SCEDA_ListMap (*table)[];
} SCEDA_HashMap;

#define SCEDA_hashmap_nth_map(map$, i$) (&((*((map$)->table))[i$]))

/** Initialise a (hash) map.

    @param map = map
    @param delete_key = delete function for keys or NULL
    @param delete_value = delete function for values or NULL
    @param match_key = equality function on keys
    @param hash_key = hash function for keys */
void SCEDA_hashmap_init(SCEDA_HashMap *map, SCEDA_delete_fun delete_key, SCEDA_delete_fun delete_value,
		  SCEDA_match_fun match_key, SCEDA_hash_fun hash_key);

/** Create a (hash) map, ready for use.

    @param delete_key = delete function for keys or NULL
    @param delete_value = delete function for values or NULL
    @param match_key = equality function on keys
    @param hash_key = hash function for keys
    
    @return the (hash) map */
SCEDA_HashMap *SCEDA_hashmap_create(SCEDA_delete_fun delete_key, SCEDA_delete_fun delete_value, 
			SCEDA_match_fun match_key, SCEDA_hash_fun hash_key);

/** Clean up a (hash) map.

    @param map = map to delete */
void SCEDA_hashmap_cleanup(SCEDA_HashMap *map);

/** Delete a (hash) map.

    @param map = map to delete */
void SCEDA_hashmap_delete(SCEDA_HashMap *map);

/** Clear a (hash) map.
    
    @param map = map to clear */
void SCEDA_hashmap_clear(SCEDA_HashMap *map);

/** Add (or replace) a binding in the map in time complexity O(1).

    @param map = map
    @param key = key to be bound
    @param value = value to bind to key
    @param old_value = old value bound to key (set by function if not NULL)

    @return 0 if new binding, 1 if binding was replaced, -1 otherwise */
int SCEDA_hashmap_put(SCEDA_HashMap *map, const void *key, const void *value, void **old_value);

/** Remove a binding in the map in time complexity O(1).

    @param map = map
    @param key = key to remove (replaced by the actual key)
    @param value = value removed (replaced by the actual value)

    @return 0 in case of success, 1 if key was unbound, -1 otherwise */
int SCEDA_hashmap_remove(SCEDA_HashMap *map, void **key, void **value);

/** Test whether the given key is bound in the map in time complexity O(1).

    @param map = map
    @param key = key

    @return TRUE if key is bound to a value, FALSE otherwise */
int SCEDA_hashmap_contains_key(SCEDA_HashMap *map, const void *key);

/** Return the value bound to the given key or NULL in time complexity O(1).

    @param map = map
    @param key = key

    @return the value bound to the key or NULL if the key was unbound */
void *SCEDA_hashmap_get(SCEDA_HashMap *map, const void *key);

/** Look up for a binding in a map in time complexity O(1).

    @param map = map
    @param key = key to look for (replaced by the actual key)
    @param value = value bound to key (filled by the function)

    @return 0 if found, -1 otherwise */
int SCEDA_hashmap_lookup(SCEDA_HashMap *map, void **key, void **value);

/** Size of the (hash) map in time complexity O(1).

    @param map = map
    
    @return size of the map 

    \hideinitializer */
#define SCEDA_hashmap_size(map$) ((map$)->size)

/** Test whether the (hash) map is empty in time complexity O(1).

    @param map = map 

    @return TRUE if empty, FALSE otherwise 

    \hideinitializer */
#define SCEDA_hashmap_is_empty(map$) (SCEDA_hashmap_size(map$) == 0)

/** Hash map iterator */
typedef struct {
  int i;
  SCEDA_HashMap *map;
  SCEDA_ListMapIterator elements;
} SCEDA_HashMapIterator;

/** Initialise a (hash) map iterator.

    @param map = map
    @param iter = map iterator 

    \hideinitializer */
#define SCEDA_hashmap_iterator_init(map$, iter$) \
  ({ SCEDA_HashMapIterator *_iter = (iter$);   \
     _iter->i = 0;                        \
     _iter->map = (map$);	            \
     SCEDA_listmap_iterator_init(SCEDA_hashmap_nth_map((_iter->map),0), &(_iter->elements)); }) 

/** Test whether there is a next data in the map iterator. 
    
    @param iter = map iterator
    
    @return TRUE if there is a "next" data, FALSE otherwise 

    \hideinitializer */
#define SCEDA_hashmap_iterator_has_next(iter$)                                                                  \
  ({ SCEDA_HashMapIterator *_iter = (iter$);                                                                  \
     while((_iter->i < _iter->map->buckets) && (!(SCEDA_listmap_iterator_has_next(&(_iter->elements))))) { \
       _iter->i++;                                                                                       \
       if(_iter->i < _iter->map->buckets)                                                              \
         SCEDA_listmap_iterator_init(SCEDA_hashmap_nth_map((_iter->map),_iter->i), &(_iter->elements));          \
     }                                                                                                     \
     (_iter->i < _iter->map->buckets) && (SCEDA_listmap_iterator_has_next(&(_iter->elements))); })

/** Return the next data of the map iterator. 
    
    @param iter = map iterator
    @param key = corresponding key (filled by the iterator)

    @return the "next" data 

    \hideinitializer */
#define SCEDA_hashmap_iterator_next(iter$, key$) (SCEDA_listmap_iterator_next(&((iter$)->elements), key$))

/** Clean up a (hash) map iterator.

    @param iter = map iterator 

    \hideinitializer */
#define SCEDA_hashmap_iterator_cleanup(iter$)             \
  ({ SCEDA_HashMapIterator *_iter = (iter$);            \
     SCEDA_listmap_iterator_cleanup(&(_iter->elements)); \
     memset(_iter, 0, sizeof(SCEDA_HashMapIterator)); }) 


#endif
