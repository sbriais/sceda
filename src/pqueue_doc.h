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

/** \page pqueues Priority queues

    Priority queues are implemented as binary heaps.

    \section pqueueapi API

    \code
    void pqueue_init(PQueue *pqueue, delete_fun delete, compare_fun compare);
    PQueue *pqueue_create(delete_fun delete, compare_fun compare);
    \endcode

    Initialise or create a priority queue. The delete function
    provided is used (when not NULL) to delete elements upon priority
    queue clean up/delete. The comparison function compares the
    priority of the elements.

    \code
    void pqueue_cleanup(PQueue *pqueue);
    void pqueue_delete(PQueue *pqueue);
    \endcode

    Clean up or delete a priority queue. Delete all the elements in
    the priority queue according to the delete function given at
    initialisation or creation.

    \code
    void pqueue_clear(PQueue *pqueue);
    \endcode

    Remove all the elements of a priority queue.

    \code
    int pqueue_insert(PQueue *pqueue, const void *data);
    \endcode

    Insert a new element in the priority queue in time O(log|pqueue|).

    \code
    int pqueue_extract(PQueue *pqueue, void **data);
    \endcode

    Extract the minimum element of the priority queue in time O(log|pqueue|).

    \code
    int pqueue_min(PQueue *pqueue, void **data);
    \endcode

    Get the minimum element of the priority queue in time O(1).

    \code
    int pqueue_size(PQueue *pqueue);
    \endcode
  
    Return the size of the priority queue.

    \code
    int pqueue_is_empty(PQueue *pqueue);
    \endcode

    Test whether the given priority queue is empty.

    \section pqueue_examples Example

    The following example creates a priority queue of tasks.

    Then it prints the tasks in ascending priority order.

    Finally the priority queue is deleted.

    \include "pqueue/main.c"

 */
