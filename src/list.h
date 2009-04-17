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
#ifndef __SCEDA_LIST_H
#define __SCEDA_LIST_H
/** \file list.h 
    \brief Linked lists implementation */

#include "common.h"
#include <string.h>

typedef struct _SCEDA_ListElt {
  void *data;
  struct _SCEDA_ListElt *next;
} SCEDA_ListElt;

typedef struct {
  int size;
  SCEDA_ListElt *head;
  SCEDA_ListElt *tail;
  SCEDA_delete_fun delete;
} SCEDA_List;

/** Initialise a list. 

    @param[in] list = list to initialise
    @param[in] delete = function to delete data in the list or NULL */
void SCEDA_list_init(SCEDA_List *list, SCEDA_delete_fun delete);

/** Create a new linked list, ready for use.

    @param[in] delete = function to delete data or NULL

    @return the linked list */
SCEDA_List *SCEDA_list_create(SCEDA_delete_fun delete);

/** Delete all the elements (and possibly their data) of a list.
    The list is uninitialised.

    @param[in] list = list to clean up */
void SCEDA_list_cleanup(SCEDA_List *list);

/** Delete a list.

    @param[in] list = list to delete */
void SCEDA_list_delete(SCEDA_List *list);

/** Clear a list.

    @param[in] list = list to clear */
void SCEDA_list_clear(SCEDA_List *list);

/** Insert data just after a list element.

    @param[in] list = list 
    @param[in] element = list element or NULL to insert at list head
    @param[in] data = data to insert

    @return 0 in case of success, -1 otherwise */
int SCEDA_list_ins_next(SCEDA_List *list, SCEDA_ListElt *element, const void *data);

/** Remove data just after a list element.

    @param[in] list = list 
    @param[in] element = list element or NULL to remove list head
    @param[out] data = pointer to the removed data

    @return 0 in case of success, -1 otherwise */
int SCEDA_list_rem_next(SCEDA_List *list, SCEDA_ListElt *element, void **data);

/** Return length of the list.

    @param[in] list = list

    @return size of the list in time complexity O(1) 

    \hideinitializer */
#define SCEDA_list_size(list$) ((list$)->size)

/** Return head element of the list 

    @param[in] list = list

    @return head element

    \hideinitializer */
#define SCEDA_list_head(list$) ((list$)->head)

/** Return tail element of the list 

    @param[in] list = list

    @return tail element

    \hideinitializer */
#define SCEDA_list_tail(list$) ((list$)->tail)

/** Return data field of a list element 

    @param type = type of data (for cast)
    @param[in] element = list element

    @return data of the element

    \hideinitializer */
#define SCEDA_list_data(type$, element$) ((type$)((element$)->data))

/** Return next element in the list 

    @param element = list element

    @return next element */
#define SCEDA_list_next(element$) ((element$)->next)

#define SCEDA_list_is_head(list$, element$) (SCEDA_list_head(list$)==(element$))
#define SCEDA_list_is_tail(element$) (SCEDA_list_next(element$) == NULL)

/** Insert data in a list.

    @param[in] list = list
    @param[in] data = data to insert

    @return 0 in case of success, -1 otherwise 

    Complexity = O(1)

    \hideinitializer */
#define SCEDA_list_add(list$, data$) (SCEDA_list_ins_next(list$, NULL, data$))

/** Insert data at end of a list.

    @param[in] list = list
    @param[in] data = data to insert

    @return 0 in case of success, -1 otherwise

    Complexity = O(1)

    \hideinitializer */
#define SCEDA_list_add_tail(list$, data$) \
  ({ SCEDA_List *_list = list$; \
     SCEDA_list_ins_next(_list, SCEDA_list_tail(_list), data$); })

/** Remove first occurence of a data from a list

    @param[in] list = list
    @param[in, out] data = data to remove (will be replaced by the actual data)
    @param[in] match = equality function

    @return 0 in case of success, -1 otherwise 

    Complexity = O(|list|) */
int SCEDA_list_remove(SCEDA_List *list, void **data, SCEDA_match_fun match);

/** Remove head of a list

    @param[in] list = list
    @param[out] data = removed data (filled by the function)

    @return 0 in case of success, -1 otherwise 

    \hideinitializer */
#define SCEDA_list_remove_head(list$, data$) (SCEDA_list_rem_next(list$, NULL, data$))

/** Test whether a data occur in a list.

    @param[in] list = list
    @param[in] data = data to look for
    @param[in] match = equality function

    @return TRUE if present, FALSE otherwise 

    Complexity = O(|list|) */
int SCEDA_list_contains(SCEDA_List *list, const void *data, SCEDA_match_fun match);

/** Look up for a data in a list.

    @param[in] list = list
    @param[in, out] data = data to look for (will be replaced by the actual data)
    @param[in] match = equality function

    @return 0 in case if found, -1 otherwise */
int SCEDA_list_lookup(SCEDA_List *list, void **data, SCEDA_match_fun match);

/** Test whether the given list is empty. 
    
    @param[in] list = list 

    @return TRUE if empty, FALSE otherwise. 

    \hideinitializer */
#define SCEDA_list_is_empty(list$) (SCEDA_list_size(list$) == 0)

/** SCEDA_List Iterator */
typedef struct {
  SCEDA_ListElt *x;
} SCEDA_ListIterator;

/** Initialise a list iterator. 

    @param[in] list = list
    @param[in] iter = list iterator 

    \hideinitializer */
#define SCEDA_list_iterator_init(list$, iter$) ((iter$)->x = SCEDA_list_head(list$))

/** Test whether there is a next data in the list iterator.

    @param[in] iter = list iterator 

    \hideinitializer */
#define SCEDA_list_iterator_has_next(iter$) ((iter$)->x != NULL)

/** Return the next data of the list iterator. 

    @param[in] iter = list iterator 

    @return the "next" data in the list 

    \hideinitializer */
#define SCEDA_list_iterator_next(iter$)	          \
  ({ SCEDA_ListIterator *_iter = (iter$);              \
     void *data = SCEDA_list_data(void *, (_iter)->x); \
     (_iter)->x = SCEDA_list_next((_iter)->x);        \
     data; })

/** Clean up a list iterator.

    @param[in] iter = list iterator 

    \hideinitializer */
#define SCEDA_list_iterator_cleanup(iter$) (memset(iter$, 0, sizeof(SCEDA_ListIterator)))

#endif
