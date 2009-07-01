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
#include <string.h>
#include "dlist.h"
#include "util.h"

void SCEDA_dlist_init(SCEDA_DList *list, SCEDA_delete_fun delete) {
  list->size = 0;
  list->head = NULL;
  list->tail = NULL;
  list->delete = delete;
}

SCEDA_DList *SCEDA_dlist_create(SCEDA_delete_fun delete) {
  SCEDA_DList *list = safe_malloc(sizeof(SCEDA_DList));
  SCEDA_dlist_init(list,delete);
  return list;
}

void SCEDA_dlist_cleanup(SCEDA_DList *list) {
  void *data = NULL;

  while(SCEDA_dlist_size(list) > 0) {
    if(SCEDA_dlist_rem(list, SCEDA_dlist_tail(list), &data) == 0) {
      if(list->delete != NULL) {
	list->delete(data);
      }
    } 
  }
  
  memset(list, 0, sizeof(SCEDA_DList));
}

void SCEDA_dlist_delete(SCEDA_DList *list) {
  SCEDA_dlist_cleanup(list);
  free(list);
}

void SCEDA_dlist_clear(SCEDA_DList *list) {
  SCEDA_delete_fun delete = list->delete;
  SCEDA_dlist_cleanup(list);
  SCEDA_dlist_init(list, delete);
}

int SCEDA_dlist_ins_next(SCEDA_DList *list, SCEDA_DListElt *element, const void *data) {
  SCEDA_DListElt *new_element = safe_malloc(sizeof(SCEDA_DListElt));

  new_element->data = (void *)data;

  if(element == NULL) {
    if(SCEDA_dlist_size(list) == 0) {
      list->tail = new_element;
    } else {
      list->head->previous = new_element;
    }
    new_element->next = list->head;
    new_element->previous = NULL;
    list->head = new_element;
  } else {
    if(SCEDA_dlist_is_tail(element)) {
      list->tail = new_element;
    } else {
      element->next->previous = new_element;
    }
    new_element->next = SCEDA_dlist_next(element);
    new_element->previous = element;
    element->next = new_element;
  }
  list->size++;
  return 0;
}

int SCEDA_dlist_ins_previous(SCEDA_DList *list, SCEDA_DListElt *element, const void *data) {
  SCEDA_DListElt *new_element = safe_malloc(sizeof(SCEDA_DListElt));

  new_element->data = (void *)data;

  if(element == NULL) {
    if(SCEDA_dlist_size(list) == 0) {
      list->tail = new_element;
    } else {
      list->head->previous = new_element;
    }
    new_element->next = list->head;
    new_element->previous = NULL;
    list->head = new_element; 
  } else {
    if(SCEDA_dlist_is_head(element)) {
      list->head = new_element;
    } else {
      element->previous->next = new_element;
    }
    new_element->next = element;
    new_element->previous = SCEDA_dlist_previous(element);
    element->previous = new_element;
  }
  list->size++;
  return 0;
}

int SCEDA_dlist_rem(SCEDA_DList *list, SCEDA_DListElt *element, void **data) {
  if((element == NULL) || (list->size == 0)) {
    return -1;
  }

  if(SCEDA_dlist_is_head(element)) {
    list->head = SCEDA_dlist_next(element);
  } else {
    element->previous->next = SCEDA_dlist_next(element);
  }

  if(SCEDA_dlist_is_tail(element)) {
    list->tail = SCEDA_dlist_previous(element);
  } else {
    element->next->previous = SCEDA_dlist_previous(element);
  }

  *data = SCEDA_dlist_data(void *, element);
  free(element);

  list->size--;
  return 0;
}
