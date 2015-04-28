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
#ifndef __SCEDA_PQUEUE_H
#define __SCEDA_PQUEUE_H
/** \file pqueue.h
    \brief Priority queues implemented as binary heap. */

#include "common.h"

/** Type of priority queues */
typedef struct {
  int size;
  int length;
  SCEDA_delete_fun delete;
  SCEDA_compare_fun compare;
  void *(*elts)[];
} SCEDA_PQueue;

/** Initialise a priority queue.

    @param[in] pqueue = priority queue to initialise
    @param[in] delete = delete function or NULL
    @param[in] compare = comparison function
 */
void SCEDA_pqueue_init(SCEDA_PQueue *pqueue, SCEDA_delete_fun delete, SCEDA_compare_fun compare);

/** Create a new priority queue, ready for use.

    @param[in] delete = delete function
    @param[in] compare = comparison function

    @return the priority queue */
SCEDA_PQueue *SCEDA_pqueue_create(SCEDA_delete_fun delete, SCEDA_compare_fun compare);

/** Clean up a priority queue.

    @param[in] pqueue = priority queue to clean up */
void SCEDA_pqueue_cleanup(SCEDA_PQueue *pqueue);

/** Delete a priority queue.

    @param[in] pqueue = priority queue to delete */
void SCEDA_pqueue_delete(SCEDA_PQueue *pqueue);

/** Clear a priority queue.

    @param[in] pqueue = priority queue to clear */
void SCEDA_pqueue_clear(SCEDA_PQueue *pqueue);

/** Insert an element in the priority queue in time complexity
    O(log|pqueue|).

    @param[in] pqueue = priority queue
    @param[in] data = data to insert

    @return 0 in case of success, -1 otherwise */
int SCEDA_pqueue_insert(SCEDA_PQueue *pqueue, const void *data);

/** Extract the minimum element of the priority queue in time
    complexity O(log|pqueue|).

    @param[in] pqueue = priority queue
    @param[out] data = dequeued data

    @return 0 in case of success, -1 otherwise */
int SCEDA_pqueue_extract(SCEDA_PQueue *pqueue, void **data);

/** Return the minimum element in the priority queue in time
    complexity O(1)

    @param[in] pqueue = priority queue
    @param[out] data = minimum element of the priority queue (filled by the function)

    @return 0 in case of success, -1 otherwise */
int SCEDA_pqueue_min(SCEDA_PQueue *pqueue, void **data);

/** Return the size of the priority queue in time complexity O(1)

    @param[in] pqueue = priority queue

    @return the size of the priority queue

    \hideinitializer */
#define SCEDA_pqueue_size(pqueue$) ((pqueue$)->size)

/** Test whether a priority queue is empty.

    @param[in] pqueue = priority queue

    @return TRUE if queue is empty, FALSE otherwise

    \hideinitializer */
#define SCEDA_pqueue_is_empty(pqueue$) (SCEDA_pqueue_size(pqueue$) == 0)

#endif
