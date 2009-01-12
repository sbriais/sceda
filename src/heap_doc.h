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

/** \page heaps Fibonacci heaps

    Fibonacci heap implementation. A Fibonacci heap is a heap data
    structure.  They have better amortized running time than binomial
    or binary heaps.

    \section heapapi API

    \code
    void heap_init(Heap *heap, delete_fun delete, compare_fun compare);
    Heap *heap_create(delete_fun delete, compare_fun compare);
    \endcode

    Initialise or create a Fibonacci heap. The delete function
    provided is used (when not NULL) to delete elements upon heap
    clean up/delete. The comparison function compares the priority of
    the elements.

    \code
    void heap_cleanup(Heap *heap);
    void heap_delete(Heap *heap);
    \endcode

    Clean up or delete a Fibonacci heap. Delete all the elements in
    the heap according to the delete function given at initialisation
    or creation.

    \code
    void heap_clear(Heap *heap);
    \endcode

    Remove all the elements of a Fibonacci heap.

    \code
    HeapElt *heap_insert(Heap *heap, const void *data);
    \endcode

    Insert a new element in a Fibonacci heap in time O(1).  

    Return a heap element that can be used for direct access
    afterwards.

    \code
    int heap_extract(Heap *heap, void **data);
    \endcode

    Extract the minimum element of a Fibonacci heap in amortized time
    O(log|heap|).

    \code
    int heap_min(Heap *heap, void **data);
    \endcode

    Get the minimum element of a Fibonacci heap in time O(1).

    \code
    Heap *heap_union(Heap *heap1, Heap *heap2);
    \endcode

    Merge the content of two Fibonacci heaps. The two source heaps are
    reinitialised to an empty state. The resulting heap contains both
    elements of heap1 and heap2.

    \code
    void heap_decrease_key(Heap *heap, HeapElt *elt);
    \endcode

    Reflect the decrease of the key of a heap element, in amortized
    time O(1).

    No verification is performed, so it must be the case that the key
    has really decreased.

    \code
    void heap_increase_key(Heap *heap, HeapElt *elt);
    \endcode

    Reflect the increase of the key of a heap element, in amortized
    time O(log|heap|).

    No verification is performed, so it must be the case that the key
    has really increased.

    \code
    void *heap_change_key(Heap *heap, HeapElt *elt, const void *data);
    \endcode

    Change the data (key) of a heap element. If the key decreases, the
    amortized time is O(1), otherwise it is O(log|heap|).

    Return the old key of the modified element.

    \code
    void *heap_remove(Heap *heap, HeapElt *elt);
    \endcode

    Remove an element from a Fibonacci heap in amortized time O(log|heap|).
    
    Return the key of the removed element.

    \code
    int heap_size(Heap *heap);
    \endcode
  
    Return the size of a Fibonacci heap.

    \code
    int heap_is_empty(Heap *heap);
    \endcode

    Test whether the given Fibonacci heap is empty.

    \code
    type heap_data(type, HeapElt *elt);
    \endcode

    Return the key (data field) of heap element.

    \section heap_examples Example

    The following example creates a heap of tasks.

    Then it prints the tasks in ascending priority order.

    Finally the heap is deleted.

    \include "heap/main.c"

 */
