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
#ifndef __SCEDA_DLIST_H
#define __SCEDA_DLIST_H
/** \file dlist.h 
    \brief Doubly Linked lists implementation */

#include "common.h"

typedef struct _SCEDA_DListElt {
  void *data;
  struct _SCEDA_DListElt *previous;
  struct _SCEDA_DListElt *next;
} SCEDA_DListElt;

typedef struct {
  int size;
  SCEDA_DListElt *head;
  SCEDA_DListElt *tail;
  SCEDA_delete_fun delete;
} SCEDA_DList;

/** Initialise a list. 

    @param[in] list = list to initialise
    @param[in] delete = function to delete data in the list or NULL */
void SCEDA_dlist_init(SCEDA_DList *list, SCEDA_delete_fun delete);

/** Create a new linked list, ready for use.

    @param[in] delete = function to delete data or NULL

    @return the linked list */
SCEDA_DList *SCEDA_dlist_create(SCEDA_delete_fun delete);

/** Delete all the elements (and possibly their data) of a list.
    The list is uninitialised.

    @param[in] list = list to clean up */
void SCEDA_dlist_cleanup(SCEDA_DList *list);

/** Delete a list.

    @param[in] list = list to delete */
void SCEDA_dlist_delete(SCEDA_DList *list);

/** Clear a list.

    @param[in] list = list to clear */
void SCEDA_dlist_clear(SCEDA_DList *list);

/** Insert data just after a list element.

    @param[in] list = list 
    @param[in] element = list element or NULL to insert at list head
    @param[in] data = data to insert

    @return 0 in case of success, -1 otherwise */
int SCEDA_dlist_ins_next(SCEDA_DList *list, SCEDA_DListElt *element, const void *data);

/** Insert data just before a list element.

    @param[in] list = list 
    @param[in] element = list element or NULL to insert at list tail
    @param[in] data = data to insert

    @return 0 in case of success, -1 otherwise */
int SCEDA_dlist_ins_previous(SCEDA_DList *list, SCEDA_DListElt *element, const void *data);

/** Remove a list element.

    @param[in] list = list 
    @param[in] element = list element to remove
    @param[out] data = pointer to the removed data

    @return 0 in case of success, -1 otherwise */
int SCEDA_dlist_rem(SCEDA_DList *list, SCEDA_DListElt *element, void **data);

/** Return length of the list.

    @param[in] list = list

    @return size of the list in time complexity O(1) 

    \hideinitializer */
#define SCEDA_dlist_size(list$) ((list$)->size)

/** Return head element of the list 

    @param[in] list = list

    @return head element

    \hideinitializer */
#define SCEDA_dlist_head(list$) ((list$)->head)

/** Return tail element of the list 

    @param[in] list = list

    @return tail element

    \hideinitializer */
#define SCEDA_dlist_tail(list$) ((list$)->tail)

/** Return data field of a list element 

    @param type = type of data (for cast)
    @param[in] element = list element

    @return data of the element

    \hideinitializer */
#define SCEDA_dlist_data(type$, element$) ((type$)((element$)->data))

/** Return next element in the list 

    @param element = list element

    @return next element */
#define SCEDA_dlist_next(element$) ((element$)->next)

/** Return previous element in the list 

    @param element = list element

    @return previous element */
#define SCEDA_dlist_previous(element$) ((element$)->previous)

#define SCEDA_dlist_is_head(element$) (SCEDA_dlist_previous(element$) == NULL)
#define SCEDA_dlist_is_tail(element$) (SCEDA_dlist_next(element$) == NULL)

#endif
