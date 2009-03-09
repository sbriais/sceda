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
#ifndef __SCEDA_STACK_H
#define __SCEDA_STACK_H
/** \file stack.h
    \brief LIFO stacks implemented as linked lists */

#include "list.h"

/** A stack is just a list. */
typedef SCEDA_List SCEDA_Stack;

/** Create a stack. 

    \hideinitializer */
#define SCEDA_stack_create SCEDA_list_create

/** Initialise a stack.

    \hideinitializer */
#define SCEDA_stack_init SCEDA_list_init

/** Clean up a stack. 

    \hideinitializer */
#define SCEDA_stack_cleanup SCEDA_list_cleanup

/** Delete a stack. 

    \hideinitializer */
#define SCEDA_stack_delete SCEDA_list_delete

/** Clear a stack.
    
    \hideinitializer */
#define SCEDA_stack_clear SCEDA_list_clear

/** Push an element onto the stack.

    @param[in] stack = stack
    @param[in] data = data to push

    @return 0 in case of success, -1 otherwise. 

    \hideinitializer */
#define SCEDA_stack_push(stack$, data$) (SCEDA_list_ins_next(stack$, NULL, data$))

/** Pop an element from the stack.

    @param[in] stack = stack
    @param[out] data = popped data

    @return 0 in case of success, -1 otherwise 

    \hideinitializer */
#define SCEDA_stack_pop(stack$, data$) (SCEDA_list_rem_next(stack$, NULL, data$))

/** Size of the stack. 

    \hideinitializer */
#define SCEDA_stack_size SCEDA_list_size

/** SCEDA_Stack is empty? 

    \hideinitializer */
#define SCEDA_stack_is_empty SCEDA_list_is_empty

/** Return the top element of the stack or NULL.

    @param type = type of elements in the stack
    @param[in] stack = stack

    @return top element or NULL if stack is empty 

    \hideinitializer */
#define SCEDA_stack_peek(type$, stack$) \
  ({ SCEDA_Stack *_stack = stack$; \
     (SCEDA_list_head(_stack) == NULL) ? (type$)NULL : SCEDA_list_data(type$, SCEDA_list_head(_stack)); })

#endif
