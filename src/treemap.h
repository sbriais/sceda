/*
   This file is part of SCEDA.
   (c) 2008-2009 Sebastien Briais
   
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
#ifndef __SCEDA_TREEMAP_H
#define __SCEDA_TREEMAP_H
/** \file treemap.h
    \brief TreeMap implementation */

#include "common.h"
#include <string.h>

typedef struct _SCEDA_TreeMapElt {
  void *key;
  void *value;
  int color;
  struct _SCEDA_TreeMapElt *parent;
  struct _SCEDA_TreeMapElt *left;
  struct _SCEDA_TreeMapElt *right;
} SCEDA_TreeMapElt;

typedef struct {
  int size;
  SCEDA_TreeMapElt *root;
  SCEDA_TreeMapElt nil;
  SCEDA_delete_fun delete_key;
  SCEDA_delete_fun delete_value;
  SCEDA_compare_fun compare_key;
} SCEDA_TreeMap;

/** Initialise a (tree) map.

    @param[in] map = map
    @param[in] delete_key = delete function for keys or NULL
    @param[in] delete_value = delete function for values or NULL
    @param[in] compare_key = comparison function on keys */
void SCEDA_treemap_init(SCEDA_TreeMap *map, SCEDA_delete_fun delete_key, 
			SCEDA_delete_fun delete_value, SCEDA_compare_fun compare_key);

/** Create a (tree) map, ready for use.

    @param[in] delete_key = delete function for keys or NULL
    @param[in] delete_value = delete function for values or NULL
    @param[in] compare_key = comparison function on keys 
    
    @return the (tree) map */
SCEDA_TreeMap *SCEDA_treemap_create(SCEDA_delete_fun delete_key, SCEDA_delete_fun delete_value, 
				    SCEDA_compare_fun compare_key);

/** Clean up a (tree) map.

    @param[in] map = map to clean up */
void SCEDA_treemap_cleanup(SCEDA_TreeMap *map);

/** Delete a (tree) map.

    @param[in] map = map to delete */
void SCEDA_treemap_delete(SCEDA_TreeMap *map);

/** Clear a (tree) map.

    @param[in] map = map to clear */
void SCEDA_treemap_clear(SCEDA_TreeMap *map);

/** Add (or replace) a binding in the map in time complexity O(log|map|).

    @param[in] map = map
    @param[in] key = key to be bound
    @param[in] value = value to bind to key
    @param[out] old_value = old value bound to key (set by function if not NULL)

    @return 0 if new binding, 1 if binding was replaced, -1 otherwise */
int SCEDA_treemap_put(SCEDA_TreeMap *map, const void *key, const void *value, void **old_value);

/** Remove a binding in the map in time complexity O(log|map|).

    @param[in] map = map
    @param[in, out] key = key to remove (replaced by the actual key)
    @param[out] value = value removed (replaced by the actual value)

    @return 0 in case of success, 1 if key was unbound, -1 otherwise */
int SCEDA_treemap_remove(SCEDA_TreeMap *map, void **key, void **value);

/** Test whether the given key is bound in the map in time complexity O(log|map|).

    @param[in] map = map
    @param[in] key = key

    @return TRUE if key is bound to a value, FALSE otherwise */
int SCEDA_treemap_contains_key(SCEDA_TreeMap *map, const void *key);

/** Return the value bound to the given key or NULL in time complexity O(log|map|).

    @param[in] map = map
    @param[in] key = key

    @return the value bound to the key or NULL if the key was unbound */
void *SCEDA_treemap_get(SCEDA_TreeMap *map, const void *key);

/** Look up for a binding in a map in time complexity O(log|map|).

    @param[in] map = map
    @param[in, out] key = key to look for (replaced by the actual key)
    @param[out] value = value bound to key (filled by the function)

    @return 0 if found, -1 otherwise */
int SCEDA_treemap_lookup(SCEDA_TreeMap *map, void **key, void **value);

/** Size of the (tree) map in time complexity O(1).

    @param[in] map = map
    
    @return size of the map 

    \hideinitializer */
#define SCEDA_treemap_size(map$) ((map$)->size)

/** Test whether the (tree) map is empty in time complexity O(1).

    @param[in] map = map 

    @return TRUE if empty, FALSE otherwise 

    \hideinitializer */
#define SCEDA_treemap_is_empty(map$) (SCEDA_treemap_size(map$) == 0)

#define SCEDA_treemap_key(type$, elt$) ((type$)((elt$)->key))
#define SCEDA_treemap_value(type$, elt$) ((type$)((elt$)->value))

/** Tree map iterator */
typedef struct {
  SCEDA_TreeMapElt *elt;
  SCEDA_TreeMap *map;
} SCEDA_TreeMapIterator;

/** Initialise a (tree) map iterator.

    @param[in] map = map
    @param[in] iter = map iterator */
void SCEDA_treemap_iterator_init(SCEDA_TreeMap *map, SCEDA_TreeMapIterator *iter);

/** Test whether there is a next data in the map iterator. 
    
    @param[in] iter = map iterator
    
    @return TRUE if there is a "next" data, FALSE otherwise */
int SCEDA_treemap_iterator_has_next(SCEDA_TreeMapIterator *iter);

/** Return the next data of the map iterator. 
    
    @param[in] iter = map iterator
    @param[out] key = corresponding key (filled by the iterator)

    @return the "next" data */
void *SCEDA_treemap_iterator_next(SCEDA_TreeMapIterator *iter, void **key);

/** Clean up the (tree) map iterator.

    @param[in] iter = map iterator */
void SCEDA_treemap_iterator_cleanup(SCEDA_TreeMapIterator *iter);

#endif
