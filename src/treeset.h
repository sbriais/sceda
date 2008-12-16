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
#ifndef SCEDA_TREESET_H
#define SCEDA_TREESET_H
/** \file treeset.h
    \brief TreeSet implementation */

#include "common.h"
#include <string.h>

typedef struct _SCEDA_TreeSetElt {
  void *data;
  int color;
  struct _SCEDA_TreeSetElt *parent;
  struct _SCEDA_TreeSetElt *left;
  struct _SCEDA_TreeSetElt *right;
} SCEDA_TreeSetElt;

typedef struct {
  int size;
  SCEDA_TreeSetElt *root;
  SCEDA_TreeSetElt nil;
  SCEDA_delete_fun delete;
  SCEDA_compare_fun compare;
} SCEDA_TreeSet;

/** Initialise a (tree) set.

    @param set = set
    @param delete = delete function or NULL
    @param compare = comparison function */
void SCEDA_treeset_init(SCEDA_TreeSet *set, SCEDA_delete_fun delete, SCEDA_compare_fun compare);

/** Create a (tree) set, ready for use.

    @param delete = delete function or NULL
    @param compare = comparison function 
    
    @return the (tree) set */
SCEDA_TreeSet *SCEDA_treeset_create(SCEDA_delete_fun delete, SCEDA_compare_fun compare);

/** Clean up a (tree) set.

    @param set = set to clean up */
void SCEDA_treeset_cleanup(SCEDA_TreeSet *set);

/** Delete a (tree) set.

    @param set = set to delete */
void SCEDA_treeset_delete(SCEDA_TreeSet *set);

/** Clear a (tree) set.

    @param set = set to clear */
void SCEDA_treeset_clear(SCEDA_TreeSet *set);

/** Insert data into a (tree) set in time complexity O(log|set|).

    @param set = set
    @param data = data to add

    @return 0 in case of success, 1 if already present, -1 otherwise */
int SCEDA_treeset_add(SCEDA_TreeSet *set, const void *data);

/** Remove a data from a set in time complexity O(log|set|).

    @param set = set
    @param data = data to remove (replaced by the actual data)

    @return 0 in case of success, 1 if not in the set, -1 otherwise */
int SCEDA_treeset_remove(SCEDA_TreeSet *set, void **data);

/** Test whether a given data belongs to a set in time complexity O(log|set|).

    @param set = set
    @param data = data to look for

    @return TRUE if present, FALSE otherwise */
int SCEDA_treeset_contains(SCEDA_TreeSet *set, const void *data);

/** Look up for a data in a set in time complexity O(log|set|).

    @param set = set
    @param data = data to look for (replaced by the actual data)

    @return 0 if found, -1 otherwise */
int SCEDA_treeset_lookup(SCEDA_TreeSet *set, void **data);

/** Size of the (tree) set in time complexity O(1).

    @param set = set
    
    @return size of the set 

    \hideinitializer */
#define SCEDA_treeset_size(set$) ((set$)->size)

/** Test whether the (tree) set is empty in time complexity O(1).

    @param set = set 

    @return TRUE if empty, FALSE otherwise 

    \hideinitializer */
#define SCEDA_treeset_is_empty(set$) (SCEDA_treeset_size(set$) == 0)

#define SCEDA_treeset_data(type$, elt$) ((type$)((elt$)->data))

/** Iterator (ascending order) */
typedef struct {
  SCEDA_TreeSetElt *elt;
  SCEDA_TreeSet *set;
} SCEDA_TreeSetIterator;

/** Initialise a (tree) set iterator.
    Elements are visited in ascending order.

    @param set = set
    @param iter = tree set iterator */
void SCEDA_treeset_iterator_init(SCEDA_TreeSet *set, SCEDA_TreeSetIterator *iter);

/** Test whether there is a next data in the set iterator. 
    
    @param iter = set iterator
    
    @return TRUE if there is a "next" data, FALSE otherwise */
int SCEDA_treeset_iterator_has_next(SCEDA_TreeSetIterator *iter);

/** Return the next data of the set iterator. 
    
    @param iter = set iterator

    @return the "next" data */
void *SCEDA_treeset_iterator_next(SCEDA_TreeSetIterator *iter);

/** Clean up the (tree) set iterator.

    @param iter = set iterator 

    \hideinitializer */
#define SCEDA_treeset_iterator_cleanup(iter$) (memset(iter$, 0, sizeof(SCEDA_TreeSetIterator)))

#endif
