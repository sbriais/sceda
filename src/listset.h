/*
   Copyright 2008, 2009 Sebastien Briais
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
#ifndef __SCEDA_LISTSET_H
#define __SCEDA_LISTSET_H
/** \file listset.h
    \brief ListSet implementation */

#include "common.h"
#include "list.h"

typedef struct {
  SCEDA_List elements;
  SCEDA_match_fun match;
} SCEDA_ListSet;

/** Initialise a (list) set.
    
    @param[in] set = set to initialise
    @param[in] delete = function to delete data in the set or NULL
    @param[in] match = equality function */
void SCEDA_listset_init(SCEDA_ListSet *set, SCEDA_delete_fun delete, SCEDA_match_fun match);

/** Create a new (list) set, ready for use.

    @param[in] delete = function to delete data in the set or NULL
    @param[in] match = equality function

    @return the (list) set */
SCEDA_ListSet *SCEDA_listset_create(SCEDA_delete_fun delete, SCEDA_match_fun match);

/** Clean up all the elements of a (list) set.
    The set is uninitialised.

    @param[in] set = set to clean up */
void SCEDA_listset_cleanup(SCEDA_ListSet *set);

/** Delete a (list) set.

    @param[in] set = set to delete */
void SCEDA_listset_delete(SCEDA_ListSet *set);

/** Clear a (list) set.

    @param[in] set = set to clear */
void SCEDA_listset_clear(SCEDA_ListSet *set);

/** Insert data in a set in time complexity O(|set|).

    @param[in] set = set
    @param[in] data = data to insert

    @return 0 if success, 1 if already in set, -1 otherwise. */
int SCEDA_listset_add(SCEDA_ListSet *set, const void *data);

#define SCEDA_listset_add_tail(set$, data$) (SCEDA_list_add_tail(SCEDA_listset_elements(set$), data$))

/** Remove data from a set in time complexity O(|set|).

    @param[in] set = set
    @param[in, out] data = data to remove (will be replaced by the actual data)

    @return 0 if success, 1 if not in the set, -1 otherwise. 

    \hideinitializer */
#define SCEDA_listset_remove(set$, data$) \
  ({ SCEDA_ListSet *_set = set$; \
     SCEDA_list_remove(SCEDA_listset_elements(_set), (data$), (_set)->match); })

#define SCEDA_listset_remove_head(set$, data$) (SCEDA_list_remove_head(SCEDA_listset_elements(set$), (data$)))

/** Test whether a given data is in the set in time complexity O(|set|).

    @param[in] set = set
    @param[in] data = data to look for
    
    @return TRUE if present, FALSE otherwise 

    \hideinitializer */
#define SCEDA_listset_contains(set$, data$) \
  ({ SCEDA_ListSet *_set = set$; \
     SCEDA_list_contains(SCEDA_listset_elements(_set), (data$), (_set)->match); })

/** Look up for a data in the set in time complexity O(|set|).

    @param[in] set = set
    @param[in, out] data = data to look for (will be replaced by the actual data)

    @return 0 if found, -1 otherwise 

    \hideinitializer */
#define SCEDA_listset_lookup(set$, data$) \
  ({ SCEDA_ListSet *_set = set$; \
     SCEDA_list_lookup(SCEDA_listset_elements(_set), (data$), (_set)->match); })

/** Return cardinal of the set in time complexity O(1).

    @param[in] set = set 

    \hideinitializer */
#define SCEDA_listset_size(set$) (SCEDA_list_size(SCEDA_listset_elements(set$)))

/** Test whether the given set is empty in time complexity O(1).

    @param[in] set = set

    @return TRUE if empty, FALSE otherwise 

    \hideinitializer */
#define SCEDA_listset_is_empty(set$) (SCEDA_listset_size(set$) == 0)

/** Return the list of elements in the set. 
    
    @param[in] set = set

    @return the list of elements
    
    \hideinitializer */
#define SCEDA_listset_elements(set$) (&((set$)->elements))

/** SCEDA_ListSet Iterator */
typedef SCEDA_ListIterator SCEDA_ListSetIterator;

/** Initialise a (list) set iterator.

    @param[in] set = set
    @param[in] iter = set iterator 

    \hideinitializer */
#define SCEDA_listset_iterator_init(set$, iter$) (SCEDA_list_iterator_init(SCEDA_listset_elements(set$), iter$))

/** Test whether there is a next data in the (list) set iterator. 

    \hideinitializer */
#define SCEDA_listset_iterator_has_next SCEDA_list_iterator_has_next

/** Return the next data of a (list) set iterator. 

    \hideinitializer */
#define SCEDA_listset_iterator_next SCEDA_list_iterator_next

/** Clean up a (list) set iterator 

    \hideinitializer */
#define SCEDA_listset_iterator_cleanup SCEDA_list_iterator_cleanup

#endif
