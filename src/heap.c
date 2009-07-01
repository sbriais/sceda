/*
   This file is part of SCEDA.
   Copyright 2008, 2009 Sebastien Briais
   
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
#include "heap.h"
#include "util.h"
#include <stdlib.h>
#include <string.h>

//#define SCEDA_heap_max_degree(n) (32)

#define heap_elt_value(x$) ((x$)->value)
#define heap_elt_key(x$) ((x$)->key)
#define compare_heap_elt(heap$, x$, y$) ((heap$)->compare_key(heap_elt_key(x$), heap_elt_key(y$)))

static inline int SCEDA_heap_max_degree(unsigned int n) {
  int res = 0;
  while(n != 0) {
    n >>= 1;
    res++;
  }
  res += res>>1;
  return res;
}

static inline void concat(SCEDA_HeapElt *elt1, SCEDA_HeapElt *elt2) {
  SCEDA_HeapElt *n1 = elt1->next;
  SCEDA_HeapElt *p2 = elt2->previous;

  elt1->next = elt2;
  elt2->previous = elt1;
  p2->next = n1;
  n1->previous = p2;
}

static inline SCEDA_HeapElt *remove_list(SCEDA_HeapElt *elt) {
  if(elt->next == elt) {
    return NULL;
  } else {
    elt->previous->next = elt->next;
    elt->next->previous = elt->previous;
    return elt->next;
  }
}

static inline void insert_root(SCEDA_Heap *heap, SCEDA_HeapElt *elt) {
  elt->next = elt;
  elt->previous = elt;

  elt->parent = NULL;

  if(heap->min == NULL) {
    heap->min = elt;
  } else {
    concat(elt, heap->min);
  }
}

static inline void remove_root(SCEDA_Heap *heap, SCEDA_HeapElt *elt) {
  heap->min = remove_list(elt);
}

static inline void remove_child(SCEDA_HeapElt *parent, SCEDA_HeapElt *elt) {
  parent->child = remove_list(elt);
  parent->degree--;
}

void SCEDA_heap_init(SCEDA_Heap *heap, 
		     SCEDA_delete_fun delete_value, SCEDA_delete_fun delete_key, 
		     SCEDA_compare_fun compare_key) {
  heap->min = NULL;
  heap->size = 0;
  heap->delete_value = delete_value;
  heap->delete_key = delete_key;
  heap->compare_key = compare_key;
}

SCEDA_Heap *SCEDA_heap_create(SCEDA_delete_fun delete_value, SCEDA_delete_fun delete_key, 
			      SCEDA_compare_fun compare_key) {
  SCEDA_Heap *heap = (SCEDA_Heap *)safe_malloc(sizeof(SCEDA_Heap));
  SCEDA_heap_init(heap, delete_value, delete_key, compare_key);
  return heap;
}

static void SCEDA_heap_cleanup_aux(SCEDA_Heap *heap, SCEDA_HeapElt *elt) {
  while(elt->child != NULL) {
    SCEDA_HeapElt *child = elt->child;
    remove_child(elt, child);
    SCEDA_heap_cleanup_aux(heap, child);
  }
  if(heap->delete_value != NULL) {
    heap->delete_value(heap_elt_value(elt));
  }
  if(heap->delete_key != NULL) {
    heap->delete_key(heap_elt_key(elt));
  }
  free(elt);
}

void SCEDA_heap_cleanup(SCEDA_Heap *heap) {
  while(heap->min != NULL) {
    SCEDA_HeapElt *root = heap->min;
    remove_root(heap, root);
    SCEDA_heap_cleanup_aux(heap, root);
  }
  memset(heap, 0, sizeof(SCEDA_Heap));
}

void SCEDA_heap_delete(SCEDA_Heap *heap) {
  SCEDA_heap_cleanup(heap);
  free(heap);
}

void SCEDA_heap_clear(SCEDA_Heap *heap) {
  SCEDA_delete_fun delete_value = heap->delete_value;
  SCEDA_delete_fun delete_key = heap->delete_key;
  SCEDA_compare_fun compare_key = heap->compare_key;
  SCEDA_heap_cleanup(heap);
  SCEDA_heap_init(heap, delete_value, delete_key, compare_key);
}

static void SCEDA_heap_insert_elt(SCEDA_Heap *heap, SCEDA_HeapElt *elt) {
  elt->child = NULL;
  
  elt->degree = 0;
  elt->marked = FALSE;
  
  insert_root(heap, elt);
  
  if(compare_heap_elt(heap, elt, heap->min) < 0) {
    heap->min = elt;
  }
  
  heap->size++;
}

SCEDA_HeapElt *SCEDA_heap_insert(SCEDA_Heap *heap, const void *value, const void *key) {
  SCEDA_HeapElt *elt = (SCEDA_HeapElt *)safe_malloc(sizeof(SCEDA_HeapElt));
  elt->value = (void *)value;
  elt->key = (void *)key;

  SCEDA_heap_insert_elt(heap, elt);

  return elt;
}

SCEDA_Heap *SCEDA_heap_union(SCEDA_Heap *heap1, SCEDA_Heap *heap2) {
  SCEDA_Heap *heap = SCEDA_heap_create(heap1->delete_value, heap1->delete_key, heap1->compare_key);

  heap->size = heap1->size + heap2->size;

  if(heap1->min == NULL) {
    heap->min = heap2->min;
  } else if(heap2->min == NULL) {
    heap->min = heap1->min;
  } else {
    concat(heap1->min, heap2->min);

    if(compare_heap_elt(heap, heap1->min, heap2->min) < 0) {
      heap->min = heap1->min;
    } else {
      heap->min = heap2->min;
    }
  }

  SCEDA_heap_init(heap1, heap->delete_value, heap->delete_key, heap->compare_key);
  SCEDA_heap_init(heap2, heap->delete_value, heap->delete_key, heap->compare_key);

  return heap;
}

static inline void fibo_link(SCEDA_Heap *heap, SCEDA_HeapElt *y, SCEDA_HeapElt *x) {
  // y has already been removed from the roots

  y->next = y;
  y->previous = y;

  y->parent = x;
  if(x->child == NULL) {
    x->child = y;
  } else {
    concat(y, x->child);
  }

  x->degree++;
  y->marked = FALSE;
}

static void fibo_consolidate(SCEDA_Heap *heap) {
  int n = SCEDA_heap_max_degree(heap->size);

  SCEDA_HeapElt *aux[n];
  int i;
  for(i = 0; i < n; i++) {
    aux[i] = NULL;
  }

  while(heap->min != NULL) {
    SCEDA_HeapElt *x = heap->min;
    remove_root(heap, x);
    int d = x->degree;
    while(aux[d] != NULL) {
      SCEDA_HeapElt *y = aux[d];
      if(compare_heap_elt(heap, y, x) < 0) {
	SCEDA_HeapElt *tmp = y;
	y = x;
	x = tmp;
      }
      fibo_link(heap, y, x);
      aux[d] = NULL;
      d++;
    }
    aux[d] = x;
  } 
  
  for(i = 0; i < n; i++) {
    SCEDA_HeapElt *x = aux[i];
    if(x != NULL) {
      insert_root(heap, x);

      if(compare_heap_elt(heap, x, heap->min) < 0) {
	heap->min = x;
      }
    }
  }
}

static SCEDA_HeapElt *SCEDA_heap_extract_elt(SCEDA_Heap *heap) {
  SCEDA_HeapElt *z = heap->min;
  if(z != NULL) {
    while(z->child != NULL) {
      SCEDA_HeapElt *child = z->child;
      remove_child(z, child);
      insert_root(heap, child);
    }

    remove_root(heap, z);
    if(heap->min != NULL) {
      fibo_consolidate(heap);
    }
    
    heap->size--;
  } 
  return z;
}

int SCEDA_heap_extract(SCEDA_Heap *heap, void **value, void **key) {
  if(SCEDA_heap_is_empty(heap)) {
    return -1;
  }
  SCEDA_HeapElt *elt = SCEDA_heap_extract_elt(heap);
  *value = heap_elt_value(elt);
  *key = heap_elt_key(elt);
  free(elt);
  return 0;
}

static inline void fibo_cut(SCEDA_Heap *heap, SCEDA_HeapElt *x, SCEDA_HeapElt *y) {
  remove_child(y, x);
  x->marked = FALSE;
  insert_root(heap, x);
}

static void fibo_cascading_cut(SCEDA_Heap *heap, SCEDA_HeapElt *y) {
  SCEDA_HeapElt *z = y->parent;
  if(z != NULL) {
    if(y->marked) {
      fibo_cut(heap, y, z);
      fibo_cascading_cut(heap, z);
    } else {
      y->marked = TRUE;
    }
  }
}

static SCEDA_HeapElt *SCEDA_heap_remove_elt(SCEDA_Heap *heap, SCEDA_HeapElt *x) {
  SCEDA_HeapElt *y = x->parent;
  if(y != NULL) {                     // simulate -infinity key for x
    fibo_cut(heap, x, y);
    fibo_cascading_cut(heap, y);
  }
  heap->min = x;
  return SCEDA_heap_extract_elt(heap);
}

int SCEDA_heap_remove(SCEDA_Heap *heap, SCEDA_HeapElt *x, void **value, void **key) {
  SCEDA_heap_remove_elt(heap, x);
  *value = heap_elt_value(x);
  *key = heap_elt_key(x);
  free(x);
  return 0;
}

void SCEDA_heap_decrease_key(SCEDA_Heap *heap, SCEDA_HeapElt *x) {
  SCEDA_HeapElt *y = x->parent;
  if((y != NULL) && (compare_heap_elt(heap, x, y) < 0)) {
    fibo_cut(heap, x, y);
    fibo_cascading_cut(heap, y);
  }
  if(compare_heap_elt(heap, x, heap->min) < 0) {
    heap->min = x;
  }
}

void SCEDA_heap_increase_key(SCEDA_Heap *heap, SCEDA_HeapElt *x) {
  SCEDA_heap_remove_elt(heap, x);
  SCEDA_heap_insert_elt(heap, x);
}

void *SCEDA_heap_change_key(SCEDA_Heap *heap, SCEDA_HeapElt *x, const void *key) {
  void *old_key = heap_elt_key(x);
  x->key = (void *)key;

  if(heap->compare_key(key, old_key) <= 0) {
    SCEDA_heap_decrease_key(heap, x);
  } else {
    SCEDA_heap_increase_key(heap, x);
  }

  return old_key;
}

int SCEDA_heap_min(SCEDA_Heap *heap, void **value, void **key) {
  if(SCEDA_heap_is_empty(heap)) {
    return -1;
  }
  *value = heap_elt_value(heap->min);
  *key = heap_elt_key(heap->min);
  return 0;
}
