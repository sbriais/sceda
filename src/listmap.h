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
#ifndef __SCEDA_LISTMAP_H
#define __SCEDA_LISTMAP_H
/** \file listmap.h
    \brief ListMap implementation */

#include "common.h"
#include <string.h>

typedef struct _SCEDA_ListMapElt {
  void *key;
  void *value;
  struct _SCEDA_ListMapElt *next;
} SCEDA_ListMapElt;

typedef struct {
  int size;
  SCEDA_ListMapElt *head;
  SCEDA_ListMapElt *tail;
  SCEDA_delete_fun delete_key;
  SCEDA_delete_fun delete_value;
  SCEDA_match_fun match_key;
} SCEDA_ListMap;

/** Initialise a (list) map. 

    @param[in] map = map to initialise
    @param[in] delete_key = function to delete keys or NULL 
    @param[in] delete_value = function to delete values or NULL 
    @param[in] match_key = equality function on keys */
void SCEDA_listmap_init(SCEDA_ListMap *map, SCEDA_delete_fun delete_key, 
			SCEDA_delete_fun delete_value, SCEDA_match_fun match_key);

/** Create a new (list) map, ready for use.

    @param[in] delete_key = function to delete keys or NULL 
    @param[in] delete_value = function to delete values or NULL 
    @param[in] match_key = equality function on keys 

    @return the (list) map */
SCEDA_ListMap *SCEDA_listmap_create(SCEDA_delete_fun delete_key, SCEDA_delete_fun delete_value,
				    SCEDA_match_fun match_key);

/** Delete all the elements (and possibly their data) of a map.
    The map is uninitialised.

    @param[in] map = map to clean up */
void SCEDA_listmap_cleanup(SCEDA_ListMap *map);

/** Delete a (list) map.

    @param[in] map = map to delete */
void SCEDA_listmap_delete(SCEDA_ListMap *map);

/** Clear a (list) map.

    @param[in] map = map to clear */
void SCEDA_listmap_clear(SCEDA_ListMap *map);

/** Add (or replace) a binding in the map in time complexity O(|map|).

    @param[in] map = map
    @param[in] key = key to be bound
    @param[in] value = value to bind to key
    @param[out] old_value = old value bound to key (set by function if not NULL)

    @return 0 if new binding, 1 if binding was replaced, -1 otherwise */
int SCEDA_listmap_put(SCEDA_ListMap *map, const void *key, const void *value, void **old_value);

int SCEDA_listmap_ins_next(SCEDA_ListMap *map, SCEDA_ListMapElt *element, const void *key, const void *value);

#define SCEDA_listmap_add_tail(map$, key$, value$) \
  ({ SCEDA_ListMap *_map = map$; \
     SCEDA_listmap_ins_next(_map, SCEDA_listmap_tail(_map), key$, value$); })

int SCEDA_listmap_rem_next(SCEDA_ListMap *map, SCEDA_ListMapElt *element, void **key, void **value);

#define SCEDA_listmap_remove_head(map$, key$, value$) (SCEDA_listmap_rem_next(map$, NULL, key$, value$))

/** Remove a binding in the map in time complexity O(|map|).

    @param[in] map = map
    @param[in, out] key = key to remove (replaced by the actual key)
    @param[out] value = value removed (replaced by the actual value)

    @return 0 in case of success, 1 if key was unbound, -1 otherwise */
int SCEDA_listmap_remove(SCEDA_ListMap *map, void **key, void **value);

/** Test whether the given key is bound in the map in time complexity O(|map|).

    @param[in] map = map
    @param[in] key = key

    @return TRUE if key is bound to a value, FALSE otherwise */
int SCEDA_listmap_contains_key(SCEDA_ListMap *map, const void *key);

/** Return the value bound to the given key or NULL in time complexity O(|map|).

    @param[in] map = map
    @param[in] key = key

    @return the value bound to the key or NULL if the key was unbound */
void *SCEDA_listmap_get(SCEDA_ListMap *map, const void *key);


/** Look up for a binding in a map in time complexity O(|map|).

    @param[in] map = map
    @param[in, out] key = key to look for (replaced by the actual key)
    @param[out] value = value bound to key (filled by the function)

    @return 0 if found, -1 otherwise */
int SCEDA_listmap_lookup(SCEDA_ListMap *map, void **key, void **value);

/** Return size of the map in time complexity O(1).

    @param[in] map = map 

    @return size of map 

    \hideinitializer */
#define SCEDA_listmap_size(map$) ((map$)->size)

#define SCEDA_listmap_head(map$) ((map$)->head)
#define SCEDA_listmap_tail(map$) ((map$)->tail)
#define SCEDA_listmap_key(type$, element$) ((type$)((element$)->key))
#define SCEDA_listmap_value(type$, element$) ((type$)((element$)->value))
#define SCEDA_listmap_next(element$) ((element$)->next)
#define SCEDA_listmap_is_head(map$, element$) (SCEDA_listmap_head(map$) == (element$))
#define SCEDA_listmap_is_tail(element$) (SCEDA_listmap_next(element$) == NULL)

/** Test whether the given map is empty in time complexity O(1).

    @param[in] map = map 

    @return TRUE if map is empty, FALSE otherwise 

    \hideinitializer */
#define SCEDA_listmap_is_empty(map$) (SCEDA_listmap_size(map$) == 0)

/** SCEDA_ListMap Iterator */
typedef struct {
  SCEDA_ListMapElt *x;
} SCEDA_ListMapIterator;

/** Initialise a (list) map iterator. 

    @param[in] map = map
    @param[in] iter = map iterator 

    \hideinitializer */
#define SCEDA_listmap_iterator_init(map$, iter$) ((iter$)->x = SCEDA_listmap_head(map$))

/** Test whether there is a next data in the map iterator.

    @param[in] iter = map iterator 

    \hideinitializer */
#define SCEDA_listmap_iterator_has_next(iter$) ((iter$)->x != NULL)

/** Return the next data of a (list) map iterator. 

    @param[in] iter = map iterator 
    @param[out] key = corresponding key (filled by the iterator).

    @return the "next" value in the map 

    \hideinitializer */
#define SCEDA_listmap_iterator_next(iter$, key$)              \
  ({ SCEDA_ListMapIterator *_iter = (iter$);                \
     *key$ = SCEDA_listmap_key(void *, (_iter)->x);          \
     void *value = SCEDA_listmap_value(void *, (_iter)->x); \
     (_iter)->x = SCEDA_listmap_next((_iter)->x);          \
     value; })

/** Clean up a (list) map iterator.

    @param[in] iter = map iterator 

    \hideinitializer */
#define SCEDA_listmap_iterator_cleanup(iter$) (memset(iter$, 0, sizeof(SCEDA_ListMapIterator)))

#endif
