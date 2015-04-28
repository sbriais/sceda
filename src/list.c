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
#include <string.h>
#include "list.h"
#include "util.h"

void SCEDA_list_init(SCEDA_List *list, SCEDA_delete_fun delete) {
  list->size = 0;
  list->head = NULL;
  list->tail = NULL;
  list->delete = delete;
}

SCEDA_List *SCEDA_list_create(SCEDA_delete_fun delete) {
  SCEDA_List *list = safe_malloc(sizeof(SCEDA_List));
  SCEDA_list_init(list, delete);
  return list;
}

void SCEDA_list_cleanup(SCEDA_List *list) {
  void *data = NULL;

  while(SCEDA_list_size(list) > 0) {
    if(SCEDA_list_rem_next(list, NULL, &data) == 0) {
      if(list->delete != NULL) {
	list->delete(data);
      }
    }
  }

  memset(list, 0, sizeof(SCEDA_List));
}

void SCEDA_list_delete(SCEDA_List *list) {
  SCEDA_list_cleanup(list);
  free(list);
}

void SCEDA_list_clear(SCEDA_List *list) {
  SCEDA_delete_fun delete = list->delete;
  SCEDA_list_cleanup(list);
  SCEDA_list_init(list, delete);
}

int SCEDA_list_ins_next(SCEDA_List *list, SCEDA_ListElt *element, const void *data) {
  SCEDA_ListElt *new_element = safe_malloc(sizeof(SCEDA_ListElt));

  new_element->data = (void *)data;

  if(element == NULL) {
    if(SCEDA_list_size(list) == 0) {
      list->tail = new_element;
    }
    new_element->next = list->head;
    list->head = new_element;
  } else {
    if(SCEDA_list_is_tail(element)) {
      list->tail = new_element;
    }
    new_element->next = SCEDA_list_next(element);
    element->next = new_element;
  }
  list->size++;
  return 0;
}

int SCEDA_list_rem_next(SCEDA_List *list, SCEDA_ListElt *element, void **data) {
  SCEDA_ListElt *old_element;

  if(SCEDA_list_size(list) == 0) {
    return -1;
  }

  if(element == NULL) {
    old_element = SCEDA_list_head(list);
    list->head = SCEDA_list_next(old_element);
  } else {
    old_element = SCEDA_list_next(element);
    if(old_element == NULL) {
      return -1;
    }
    element->next = SCEDA_list_next(old_element);
  }

  if(SCEDA_list_is_tail(old_element)) {
    list->tail = element;
  }

  *data = SCEDA_list_data(void *, old_element);
  free(old_element);

  list->size--;
  return 0;
}

int SCEDA_list_remove(SCEDA_List *list, void **data, SCEDA_match_fun match) {
  SCEDA_ListElt *previous = NULL;
  SCEDA_ListElt *x = SCEDA_list_head(list);

  while(x != NULL) {
    if(match(SCEDA_list_data(void *, x), *data)) {
      break;
    }
    previous = x;
    x = SCEDA_list_next(x);
  }

  if(x != NULL) {
    return SCEDA_list_rem_next(list, previous, data);
  } else {
    return -1;
  }
}

int SCEDA_list_contains(SCEDA_List *list, const void *data, SCEDA_match_fun match) {
  SCEDA_ListElt *x = SCEDA_list_head(list);

  while(x != NULL) {
    if(match(SCEDA_list_data(void *, x), data)) {
      return TRUE;
    }
    x = SCEDA_list_next(x);
  }

  return FALSE;
}

int SCEDA_list_lookup(SCEDA_List *list, void **data, SCEDA_match_fun match) {
  SCEDA_ListElt *x = SCEDA_list_head(list);

  while(x != NULL) {
    if(match(SCEDA_list_data(void *, x), *data)) {
      *data = SCEDA_list_data(void *, x);
      return 0;
    }
    x = SCEDA_list_next(x);
  }

  return -1;
}
