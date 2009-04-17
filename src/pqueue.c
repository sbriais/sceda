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
#include <string.h>
#include "pqueue.h"
#include "util.h"

#define father(i) ((i-1)/2)
#define left(i) (2*(i)+1)
#define right(i) (2*(i)+2)

#define nth_elt(table, i) ((*(table))[(i)])

#define SCEDA_pqueue_elt(pqueue, i) (nth_elt((pqueue)->elts, i))

void SCEDA_pqueue_init(SCEDA_PQueue *pqueue, SCEDA_delete_fun delete, SCEDA_compare_fun compare) {
  pqueue->size = 0;
  pqueue->length = SCEDA_DFLT_SIZE;
  pqueue->delete = delete;
  pqueue->compare = compare;
  pqueue->elts = safe_calloc(pqueue->length, sizeof(void *));
}

SCEDA_PQueue *SCEDA_pqueue_create(SCEDA_delete_fun delete, SCEDA_compare_fun compare) {
  SCEDA_PQueue *pqueue = (SCEDA_PQueue *)safe_malloc(sizeof(SCEDA_PQueue));
  SCEDA_pqueue_init(pqueue, delete, compare);
  return pqueue;
}

void SCEDA_pqueue_cleanup(SCEDA_PQueue *pqueue) {
  SCEDA_delete_fun delete = pqueue->delete;

  if(delete != NULL) {
    int i;
    for(i = 0; i < pqueue->size; i++) {
      delete(SCEDA_pqueue_elt(pqueue, i));
    }
  }

  free(pqueue->elts);
  memset(pqueue, 0, sizeof(SCEDA_PQueue));
}

void SCEDA_pqueue_delete(SCEDA_PQueue *pqueue) {
  SCEDA_pqueue_cleanup(pqueue);
  free(pqueue);
}

void SCEDA_pqueue_clear(SCEDA_PQueue *pqueue) {
  SCEDA_delete_fun delete = pqueue->delete;
  SCEDA_compare_fun compare = pqueue->compare;
  SCEDA_pqueue_cleanup(pqueue);
  SCEDA_pqueue_init(pqueue, delete, compare);
}

static void resize_pqueue(SCEDA_PQueue *pqueue, const int length) {
  if((length <= 1) || (length == pqueue->length)) {
    return;
  }

  pqueue->elts = safe_realloc(pqueue->elts, length * sizeof(void *));
  pqueue->length = length;
}

int SCEDA_pqueue_insert(SCEDA_PQueue *pqueue, const void *data) {
  int i;
  SCEDA_compare_fun compare = pqueue->compare;

  i = pqueue->size;
  pqueue->size++;

  if(pqueue->size > pqueue->length) {
    resize_pqueue(pqueue, 1+2*pqueue->length);
  }

  while(i != 0) {
    int father = father(i);
    if(compare(SCEDA_pqueue_elt(pqueue, father), data) < 0) {
      break;
    }
    SCEDA_pqueue_elt(pqueue, i) = SCEDA_pqueue_elt(pqueue, father);
    i = father;
  }
  SCEDA_pqueue_elt(pqueue, i) = (void *)data;
  return 0;
}

int SCEDA_pqueue_extract(SCEDA_PQueue *pqueue, void **data) {
  int i;
  SCEDA_compare_fun compare = pqueue->compare;

  if(SCEDA_pqueue_is_empty(pqueue)) {
    return -1;
  }
  *data = SCEDA_pqueue_elt(pqueue, 0);
  pqueue->size--;
  void *ndata = SCEDA_pqueue_elt(pqueue, pqueue->size);

  for(i=0;;) {
    int l = left(i);
    int r = right(i);
    int min;
    void *mindata;
    if((l < pqueue->size) && 
       (compare(SCEDA_pqueue_elt(pqueue, l), ndata) < 0)) {
      min = l;
      mindata = SCEDA_pqueue_elt(pqueue,l);
    } else {
      min = i;
      mindata = ndata;
    }
    if((r < pqueue->size) && 
       (compare(SCEDA_pqueue_elt(pqueue, r), mindata) < 0)) {
      min = r;
      mindata = SCEDA_pqueue_elt(pqueue,r);
    }
    if(i == min) {
      break;
    }
    SCEDA_pqueue_elt(pqueue, i) = mindata;
    i = min;
  }
  SCEDA_pqueue_elt(pqueue,i) = ndata;
  
  if(4*pqueue->size < pqueue->length) {
    resize_pqueue(pqueue, 1+pqueue->length/2);
  }

  return 0;
}

int SCEDA_pqueue_min(SCEDA_PQueue *pqueue, void **data) {
  if(SCEDA_pqueue_is_empty(pqueue)) {
    return -1;
  }
  *data = SCEDA_pqueue_elt(pqueue, 0);
  return 0;
}
