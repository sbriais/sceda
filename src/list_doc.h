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

/** \page lists Linked lists, Stacks, Queues

    SCEDA provides an implementation of linked lists. Stacks (LIFO)
    and Queues (FIFO) are implemented on top of this basic data
    structure.

    \section listsapi API
    \subsection llistssubsec Linked lists

    \code
    void list_init(List *list, delete_fun delete);
    List *list_create(delete_fun delete);
    \endcode

    Initialise or create a linked list. The delete function provided
    is used (when not NULL) to delete list elements upon
    clean up/delete.

    \code
    void list_cleanup(List *list);
    void list_delete(List *list);
    \endcode

    Clean up or delete a list. Delete all the elements in the list
    according to the delete function given at initialisation or
    creation.

    \code
    void list_clear(List *list);
    \endcode

    Remove all the elements from the list.

    \code
    int list_add(List *list, const void *data);
    \endcode

    Insert a new element at beginning of the list in time complexity
    O(1).

    Return 0 in case of success, -1 otherwise (should never happen).

    \code
    int list_add_tail(List *list, const void *data);
    \endcode

    Insert a new element at the end of the list in time complexity
    O(1).

    Return 0 in case of success, -1 otherwise (should never happen).

    \code
    int list_remove(List *list, void **data, match_fun match);
    \endcode

    Remove the first element of the list that is equal to given data,
    according to match function, in time complexity O(|list|). Data is
    replaced by the removed data.

    Return 0 if a data has been removed, 1 if data was not in the
    list, -1 in case of error.

    \code
    int list_remove_head(List *list, void **data);
    \endcode

    Remove head element of the list, in time complexity O(1).

    Return 0 in case of success, or -1 if the list was empty.

    \code
    int list_contains(List *list, const void *data, match_fun match);
    \endcode

    Time complexity is O(|list|).

    Test whether a given element belongs to the list. 

    \code
    int list_lookup(List *list, void **data, match_fun match);
    \endcode

    Look up for a data in the list in time complexity O(|list|).

    \code
    int list_is_empty(List *list);
    \endcode

    Test whether the given list is empty in time complexity O(1).

    \code
    int list_size(List *list);
    \endcode

    Return the size of the list in time complexity O(1).

    \code
    void list_iterator_init(List *list, ListIterator *iter);
    int list_iterator_has_next(ListIterator *iter);
    void *list_iterator_next(ListIterator *iter);
    void list_iterator_cleanup(ListIterator *iter);
    \endcode

    Iterator over the list elements.

    \code
    ListElt *list_head(List *list);
    ListElt *list_tail(List *list);
    ListElt *list_next(ListElt *elt);
    int list_ins_next(List *list, ListElt *elt, const void *data);
    int list_rem_next(List *list, ListElt *elt, void **data);
    type list_data(type, ListElt *elt);
    \endcode

    Low level functions to manipulate lists and their structure.

    \subsection stacks Stacks

    \code
    void stack_init(Stack *stack, delete_fun delete);
    Stack *stack_create(delete_fun delete);
    \endcode

    Initialise or create a stack. The delete function provided
    is used (when not NULL) to delete stack elements upon
    clean up/delete.

    \code
    void stack_cleanup(Stack *stack);
    void stack_delete(Stack *stack);
    \endcode

    Delete or delete a stack. Delete all the elements in the stack
    according to the delete function given at initialisation or
    creation.

    \code
    void stack_clear(Stack *stack);
    \endcode

    Remove all the elements from a stack.

    \code
    int stack_push(Stack *stack, const void *data);
    \endcode

    Push an element onto the stack in time complexity O(1).

    \code
    int stack_pop(Stack *stack, void **data);
    \endcode

    Pop the top element of the stack in time complexity O(1).

    \code
    type stack_peek(type, Stack *stack);
    \endcode

    Return the top element of the stack in time complexity O(1), or
    NULL if stack is empty.
    
    \code
    int stack_size(Stack *stack);
    \endcode

    Return the size of the stack.

    \code
    int stack_is_empty(Stack *stack);
    \endcode

    Test whether the given stack is empty.

    \subsection queues Queues

    \code
    void queue_init(Queue *queue, delete_fun delete);
    Queue *queue_create(delete_fun delete);
    \endcode

    Initialise or create a queue. The delete function provided
    is used (when not NULL) to delete queue elements upon
    clean up/delete.

    \code
    void queue_cleanup(Queue *queue);
    void queue_delete(Queue *queue);
    \endcode

    Delete or delete a queue. Delete all the elements in the queue
    according to the delete function given at initialisation or
    creation.

    \code
    void queue_clear(Queue *queue);
    \endcode

    Remove all the elements from a queue.

    \code
    int queue_enqueue(Queue *queue, const void *data);
    \endcode

    Enqueue an element in the queue in time complexity O(1).

    \code
    int queue_dequeue(Queue *queue, void **data);
    \endcode

    Dequeue the top element of the queue in time complexity O(1).

    \code
    type queue_peek(type, Queue *queue);
    \endcode

    Return the top element of the queue in time complexity O(1), or
    NULL if queue is empty.
    
    \code
    int queue_size(Queue *queue);
    \endcode

    Return the size of the queue.

    \code
    int queue_is_empty(Queue *queue);
    \endcode

    Test whether the given queue is empty.

    \section list_examples Example

    \subsection list_exa List example

    The following example creates a list of Integers (Integers are
    just boxed ints, this is to allow coercion to "void *")

    It then print the list content, using a list iterator.

    Finally, the list is deleted.

    \include "list/main.c"

    \subsection stack_exa Stack example

    The following example creates a stack of Integers.

    The stack elements are then removed from the stack and printed in
    LIFO order.

    Finally the stack is deleted.

    \include "stack/main.c"

    \subsection queue_exa Queue example

    The following example creates a queue of Integers.

    The queue elements are then removed from the queue and printed in
    FIFO order.

    Finally the queue is deleted.

    \include "queue/main.c"
    
 */
