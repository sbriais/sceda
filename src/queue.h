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
#ifndef __SCEDA_QUEUE_H
#define __SCEDA_QUEUE_H
/** \file queue.h
    \brief queues implemented as linked list */

#include "list.h"

/** A queue is just a list. */
typedef SCEDA_List SCEDA_Queue;

/** Create a queue. 

    \hideinitializer */
#define SCEDA_queue_create SCEDA_list_create

/** Initialise a queue. 

    \hideinitializer */
#define SCEDA_queue_init SCEDA_list_init

/** Clean up a queue.

    \hideinitializer */
#define SCEDA_queue_cleanup SCEDA_list_cleanup

/** Delete a queue. 

    \hideinitializer */
#define SCEDA_queue_delete SCEDA_list_delete

/** Clear a queue.

    \hideinitializer */
#define SCEDA_queue_clear SCEDA_list_clear

/** Enqueue an element. 
    
    @param[in] queue = queue
    @param[in] data = data to enqueue

    @return 0 in case of success, -1 otherwise 

    \hideinitializer */
#define SCEDA_queue_enqueue(queue$, data$) \
  ({ SCEDA_Queue *_queue = queue$; \
     SCEDA_list_ins_next(_queue, SCEDA_list_tail(_queue), data$); })

/** Dequeue an element.

    @param[in] queue = queue
    @param[out] data = dequeued data

    @return 0 in case of success, -1 otherwise 

    \hideinitializer */
#define SCEDA_queue_dequeue(queue$, data$) (SCEDA_list_rem_next(queue$, NULL, data$))

/** Give the size of the queue. 

    \hideinitializer */
#define SCEDA_queue_size SCEDA_list_size

/** SCEDA_Queue is empty? 

    \hideinitializer */
#define SCEDA_queue_is_empty SCEDA_list_is_empty

/** Return the top element of the queue or NULL. 

    @param type = type of queue elements
    @param[in] queue = queue 

    @return top element of the queue or NULL if queue is empty 

    \hideinitializer */
#define SCEDA_queue_peek(type$, queue$) \
  ({ SCEDA_Queue *_queue = queue$; \
     (SCEDA_list_head(_queue) == NULL) ? (type$)NULL : SCEDA_list_data(type$, SCEDA_list_head(_queue)); })

#endif
