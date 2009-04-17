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
#include "listmap.h"
#include "util.h"

void SCEDA_listmap_init(SCEDA_ListMap *map, SCEDA_delete_fun delete_key,
		  SCEDA_delete_fun delete_value, SCEDA_match_fun match_key) {
  map->size = 0;
  map->head = NULL;
  map->tail = NULL;
  map->delete_key = delete_key;
  map->delete_value = delete_value;
  map->match_key = match_key;
}

SCEDA_ListMap *SCEDA_listmap_create(SCEDA_delete_fun delete_key, SCEDA_delete_fun delete_value,
			SCEDA_match_fun match_key) {
  SCEDA_ListMap *map = safe_malloc(sizeof(SCEDA_ListMap));
  SCEDA_listmap_init(map, delete_key, delete_value, match_key);
  return map;
}

void SCEDA_listmap_cleanup(SCEDA_ListMap *map) {
  void *key = NULL;
  void *value = NULL;

  while(SCEDA_listmap_size(map) > 0) {
    if(SCEDA_listmap_rem_next(map, NULL, &key, &value) == 0) {
      if(map->delete_key != NULL) {
	map->delete_key(key);
      }
      if(map->delete_value != NULL) {
	map->delete_value(value);
      }
    } 
  }

  memset(map, 0, sizeof(SCEDA_ListMap));
}

void SCEDA_listmap_delete(SCEDA_ListMap *map) {
  SCEDA_listmap_cleanup(map);
  free(map);
}

void SCEDA_listmap_clear(SCEDA_ListMap *map) {
  SCEDA_delete_fun delete_key = map->delete_key;
  SCEDA_delete_fun delete_value = map->delete_value;
  SCEDA_match_fun match_key = map->match_key;
  SCEDA_listmap_cleanup(map);
  SCEDA_listmap_init(map, delete_key, delete_value, match_key);
}

int SCEDA_listmap_ins_next(SCEDA_ListMap *map, SCEDA_ListMapElt *element, const void *key, const void *value) {
  SCEDA_ListMapElt *new_element = safe_malloc(sizeof(SCEDA_ListMapElt));

  new_element->key = (void *)key;
  new_element->value = (void *)value;

  if(element == NULL) {
    if(SCEDA_listmap_size(map) == 0) {
      map->tail = new_element;
    }
    new_element->next = map->head;
    map->head = new_element;
  } else {
    if(SCEDA_listmap_is_tail(element)) {
      map->tail = new_element;
    }
    new_element->next = SCEDA_listmap_next(element);
    element->next = new_element;
  }

  map->size++;
  return 0;
}

int SCEDA_listmap_rem_next(SCEDA_ListMap *map, SCEDA_ListMapElt *element, 
			    void **key, void **value) {
  SCEDA_ListMapElt *old_element;

  if(SCEDA_listmap_size(map) == 0) {
    return -1;
  }

  if(element == NULL) {
    old_element = SCEDA_listmap_head(map);
    map->head = SCEDA_listmap_next(old_element);
  } else {
    old_element = SCEDA_listmap_next(element);
    if(old_element == NULL) {
      return -1;
    }
    element->next = SCEDA_listmap_next(old_element);
  }

  if(SCEDA_listmap_is_tail(old_element)) {
    map->tail = element;
  }

  *key = SCEDA_listmap_key(void *, old_element);
  *value = SCEDA_listmap_value(void *, old_element);
  free(old_element);

  map->size--;
  return 0;
}

int SCEDA_listmap_put(SCEDA_ListMap *map, const void *key, const void *value, void **old_value) {
  SCEDA_match_fun match = map->match_key; 

  SCEDA_ListMapElt *x = SCEDA_listmap_head(map);
  while(x != NULL) {
    if(match(SCEDA_listmap_key(void *, x), key)) {
      if(old_value != NULL) {
	*old_value = SCEDA_listmap_value(void *, x);
      }
      x->value = (void *)value;
      return 1;
    }
    x = SCEDA_listmap_next(x);
  }

  return SCEDA_listmap_ins_next(map, NULL, key, value);
}

int SCEDA_listmap_contains_key(SCEDA_ListMap *map, const void *key) {
  SCEDA_match_fun match = map->match_key;

  SCEDA_ListMapElt *x = SCEDA_listmap_head(map);
  while(x != NULL) {
    if(match(SCEDA_listmap_key(void *, x), key)) {
      return TRUE;
    }
    x = SCEDA_listmap_next(x);
  }

  return FALSE;
}

void *SCEDA_listmap_get(SCEDA_ListMap *map, const void *key) {
  SCEDA_match_fun match = map->match_key;

  SCEDA_ListMapElt *x = SCEDA_listmap_head(map);
  while(x != NULL) {
    if(match(SCEDA_listmap_key(void *, x), key)) {
      return SCEDA_listmap_value(void *, x);
    }
    x = SCEDA_listmap_next(x);
  }

  return NULL;
}

int SCEDA_listmap_remove(SCEDA_ListMap *map, void **key, void **value) {
  SCEDA_match_fun match = map->match_key;

  SCEDA_ListMapElt *previous = NULL;
  SCEDA_ListMapElt *x = SCEDA_listmap_head(map);
  while(x != NULL) {
    if(match(SCEDA_listmap_key(void *, x), *key)) {
      break;
    }
    previous = x;
    x = SCEDA_listmap_next(x);
  }

  if(x != NULL) {
    return SCEDA_listmap_rem_next(map, previous, key, value);
  } else {
    return 1;
  }
}

int SCEDA_listmap_lookup(SCEDA_ListMap *map, void **key, void **value) {
  SCEDA_match_fun match = map->match_key;

  SCEDA_ListMapElt *x = SCEDA_listmap_head(map);
  while(x != NULL) {
    if(match(SCEDA_listmap_key(void *, x), *key)) {
      *key = SCEDA_listmap_key(void *, x);
      *value = SCEDA_listmap_value(void *, x);
      return 0;
    }
    x = SCEDA_listmap_next(x);
  }

  return -1;
}
