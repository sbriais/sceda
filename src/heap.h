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
#ifndef __SCEDA_HEAP_H
#define __SCEDA_HEAP_H
/** \file heap.h
    \brief Fibonacci heaps. */

#include "common.h"

/** An element of a Fibonacci heap */
typedef struct _SCEDA_HeapElt {
  struct _SCEDA_HeapElt *parent;
  struct _SCEDA_HeapElt *child;
  struct _SCEDA_HeapElt *previous;
  struct _SCEDA_HeapElt *next;
  int degree;
  int marked;
  void *data;
  void *priority;
} SCEDA_HeapElt;

typedef struct {
  SCEDA_HeapElt *min;
  int size;
  SCEDA_delete_fun delete_data;
  SCEDA_delete_fun delete_priority;
  SCEDA_compare_fun compare_priority;
} SCEDA_Heap;

/** Initialise a Fibonacci heap.

    @param[in] heap = heap to initialise
    @param[in] delete_data = delete data function
    @param[in] delete_priority = delete priority function
    @param[in] compare_priority = compare priority function */
void SCEDA_heap_init(SCEDA_Heap *heap, 
		     SCEDA_delete_fun delete_data, SCEDA_delete_fun delete_priority, 
		     SCEDA_compare_fun compare_priority);

/** Create a Fibonacci heap.

    @param[in] delete_data = delete data function
    @param[in] delete_priority = delete priority function
    @param[in] compare_priority = compare priority function 

    @return a Fibonacci heap */
SCEDA_Heap *SCEDA_heap_create(SCEDA_delete_fun delete_data, SCEDA_delete_fun delete_priority, 
			      SCEDA_compare_fun compare_priority);

/** Clean up a Fibonacci heap.

    @param[in] heap = heap to clean up */
void SCEDA_heap_cleanup(SCEDA_Heap *heap);

/** Delete a Fibonacci heap.

    @param[in] heap = heap to delete */
void SCEDA_heap_delete(SCEDA_Heap *heap);

/** Clean a Fibonacci heap.

    @param[in] heap = heap to clear */
void SCEDA_heap_clear(SCEDA_Heap *heap);

/** Insert an element into a Fibonacci heap in time complexity O(1).

    @param[in] heap = heap 
    @param[in] data = data to insert

    @return a heap element (for direct access) */
SCEDA_HeapElt *SCEDA_heap_insert(SCEDA_Heap *heap, const void *data, const void *priority);

/** Merge two Fibonacci heap in time complexity O(1).

    @param[in] heap1 = first heap (reinitialised)
    @param[in] heap2 = second heap (reinitialised)

    @return a heap that is the union of heap1 and heap2, which are
    reinitialised to an empty state after the call. */
SCEDA_Heap *SCEDA_heap_union(SCEDA_Heap *heap1, SCEDA_Heap *heap2);

/** Extract the minimum element of a Fibonacci heap in amortized time O(log|heap|).

    @param[in] heap = heap
    @param[out] data = data of extracted element
    @param[out] priority = priority of extracted element 

    @return 0 in case of success, -1 otherwise */
int SCEDA_heap_extract(SCEDA_Heap *heap, void **data, void **priority);

/** Reflect the decrease of the key of a heap element, in amortized
    time O(1).

    @param[in] heap = heap
    @param[in] x = heap element whose key has decreased */
void SCEDA_heap_decrease_key(SCEDA_Heap *heap, SCEDA_HeapElt *x);

/** Reflect the increase of the key of a heap element, in amortized
    time O(log|heap|).

    @param[in] heap = heap
    @param[in] x = heap element whose key has increased */
void SCEDA_heap_increase_key(SCEDA_Heap *heap, SCEDA_HeapElt *x);

/** Modify the priority of a heap element.

    @param[in] heap = heap
    @param[in] elt = heap element
    @param[in] priority = new data 

    @return the old priority */
void *SCEDA_heap_change_key(SCEDA_Heap *heap, SCEDA_HeapElt *elt, const void *priority);

/** Remove an element from a Fibonacci heap, in amortized time O(log|heap|).

    @param[in] heap = heap
    @param[in] elt = heap element to remove
    @param[out] data = data of removed element
    @param[out] priority = priority of removed element

    @return 0 in case of success, -1 otherwise */
int SCEDA_heap_remove(SCEDA_Heap *heap, SCEDA_HeapElt *elt, void **data, void **priority);

/** Return the size of a heap.

    @param[in] heap = heap

    @return the size of the heap 

    \hideinitializer */
#define SCEDA_heap_size(heap$) ((heap$)->size)

/** Test whether a heap is empty.

    @param[in] heap = heap

    @return TRUE if heap is empty, FALSE otherwise 

    \hideinitializer */
#define SCEDA_heap_is_empty(heap$) (SCEDA_heap_size(heap$) == 0)

/** Return the minimum element of a heap in time complexity O(1).

    @param[in] heap = heap
    @param[out] data = data of minimum element
    @param[out] priority = priority of minimum element

    @return 0 in case of success, -1 otherwise */
int SCEDA_heap_min(SCEDA_Heap *heap, void **data, void **priority);

#endif
