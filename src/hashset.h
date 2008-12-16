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
#ifndef __SCEDA_HASHSET_H
#define __SCEDA_HASHSET_H
/** \file hashset.h
    \brief HashSet implementation */

#include "common.h"
#include "listset.h"
#include <string.h>

typedef struct {
  int buckets;
  int size;
  SCEDA_delete_fun delete;
  SCEDA_match_fun match;
  SCEDA_hash_fun hash;
  SCEDA_ListSet (*table)[];
} SCEDA_HashSet;

#define SCEDA_hashset_nth_set(set$, i$) (&((*((set$)->table))[i$]))

/** Initialise a (hash) set.

    @param set = set
    @param delete = delete function or NULL
    @param match = equality function
    @param hash = hash function */
void SCEDA_hashset_init(SCEDA_HashSet *set, SCEDA_delete_fun delete, 
		  SCEDA_match_fun match, SCEDA_hash_fun hash);

/** Create a (hash) set, ready for use.

    @param delete = delete function or NULL
    @param match = equality function
    @param hash = hash function 
    
    @return the (hash) set */
SCEDA_HashSet *SCEDA_hashset_create(SCEDA_delete_fun delete, SCEDA_match_fun match,
			SCEDA_hash_fun hash);

/** Clean up a (hash) set.

    @param set = set to clean up */
void SCEDA_hashset_cleanup(SCEDA_HashSet *set);

/** Delete a (hash) set.

    @param set = set to delete */
void SCEDA_hashset_delete(SCEDA_HashSet *set);

/** Clear a (hash) set.

    @param set = set to clear */
void SCEDA_hashset_clear(SCEDA_HashSet *set);

/** Insert data into a (hash) set in time complexity O(1).

    @param set = set
    @param data = data to add

    @return 0 in case of success, 1 if already present, -1 otherwise */
int SCEDA_hashset_add(SCEDA_HashSet *set, const void *data);

/** Remove a data from a set in time complexity O(1).

    @param set = set
    @param data = data to remove (replaced by the actual data)

    @return 0 in case of success, 1 if not in the set, -1 otherwise */
int SCEDA_hashset_remove(SCEDA_HashSet *set, void **data);

/** Test whether a given data belongs to a set in time complexity O(1).

    @param set = set
    @param data = data to look for

    @return TRUE if present, FALSE otherwise */
int SCEDA_hashset_contains(SCEDA_HashSet *set, const void *data);

/** Look up for a data in a set in time complexity O(1).

    @param set = set
    @param data = data to look for (replaced by the actual data)

    @return 0 if found, -1 otherwise */
int SCEDA_hashset_lookup(SCEDA_HashSet *set, void **data);

/** Size of the (hash) set in time complexity O(1).

    @param set = set
    
    @return size of the set 

    \hideinitializer  */
#define SCEDA_hashset_size(set$) ((set$)->size)

/** Test whether the (hash) set is empty in time complexity O(1).

    @param set = set 

    @return TRUE if empty, FALSE otherwise 

    \hideinitializer */
#define SCEDA_hashset_is_empty(set$) (SCEDA_hashset_size(set$) == 0)

/** Hash set iterator */
typedef struct {
  int i;
  SCEDA_HashSet *set;
  SCEDA_ListSetIterator elements;
} SCEDA_HashSetIterator;

/** Initialise a (hash) set iterator.

    @param set = set
    @param iter = set iterator 

    \hideinitializer */
#define SCEDA_hashset_iterator_init(set$, iter$) \
  ({ SCEDA_HashSetIterator *_iter = (iter$);   \
     _iter->i = 0;                        \
     _iter->set = (set$);	            \
     SCEDA_listset_iterator_init(SCEDA_hashset_nth_set((_iter->set),0), &(_iter->elements)); }) 

/** Test whether there is a next data in the set iterator. 
    
    @param iter = set iterator
    
    @return TRUE if there is a "next" data, FALSE otherwise 

    \hideinitializer */
#define SCEDA_hashset_iterator_has_next(iter$)                                                                  \
  ({ SCEDA_HashSetIterator *_iter = (iter$);                                                                  \
     while((_iter->i < _iter->set->buckets) && (!(SCEDA_listset_iterator_has_next(&(_iter->elements))))) { \
       _iter->i++;                                                                                       \
       if(_iter->i < _iter->set->buckets) {                                                            \
	 SCEDA_listset_iterator_cleanup(&(_iter->elements));		                                   \
         SCEDA_listset_iterator_init(SCEDA_hashset_nth_set((_iter->set),_iter->i), &(_iter->elements));          \
       }                                                                                                   \
     }                                                                                                     \
     (_iter->i < _iter->set->buckets) && (SCEDA_listset_iterator_has_next(&(_iter->elements))); })

/** Return the next data of a set iterator. 
    
    @param iter = set iterator

    @return the "next" data 

    \hideinitializer */
#define SCEDA_hashset_iterator_next(iter$) (SCEDA_listset_iterator_next(&((iter$)->elements)))

/** Clean up a (hash) set iterator.

    @param iter = set iterator 

    \hideinitializer */
#define SCEDA_hashset_iterator_cleanup(iter$)             \
  ({ SCEDA_HashSetIterator *_iter = (iter$);            \
     SCEDA_listset_iterator_cleanup(&(_iter->elements)); \
     memset(_iter, 0, sizeof(SCEDA_HashSetIterator)); })

#endif
