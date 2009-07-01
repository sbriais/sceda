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
#include "treemap.h"
#include "util.h"

#define RED 0
#define BLACK 1

#define SCEDA_treemap_root(map) ((map)->root)
#define SCEDA_treemap_nil(map) (&((map)->nil))

static SCEDA_TreeMapElt *SCEDA_treemap_min(SCEDA_TreeMap *map, SCEDA_TreeMapElt *x) {
  while(x != SCEDA_treemap_nil(map)) {
    if(x->left == SCEDA_treemap_nil(map)) {
      break;
    }
    x = x->left;
  }
  return x;
}

static SCEDA_TreeMapElt *SCEDA_treemap_succ(SCEDA_TreeMap *map, SCEDA_TreeMapElt *x) {
  if(x->right != SCEDA_treemap_nil(map)) {
    // get min of right subtree
    x = x->right;
    return SCEDA_treemap_min(map, x);
  } else {
    // get root of left subtree
    SCEDA_TreeMapElt *y = x->parent;
    while((y != SCEDA_treemap_nil(map)) && (x == y->right)) {
      x = y;
      y = x->parent;
    }
    return y;
  }
}

void SCEDA_treemap_init(SCEDA_TreeMap *map, SCEDA_delete_fun delete_key, SCEDA_delete_fun delete_value, SCEDA_compare_fun compare_key) {
  map->size = 0;
  map->root = SCEDA_treemap_nil(map);
  map->delete_key = delete_key;
  map->delete_value = delete_value;
  map->compare_key = compare_key;
  map->nil.color = BLACK;
}

SCEDA_TreeMap *SCEDA_treemap_create(SCEDA_delete_fun delete_key, SCEDA_delete_fun delete_value, SCEDA_compare_fun compare_key) {
  SCEDA_TreeMap *map = (SCEDA_TreeMap *)safe_malloc(sizeof(SCEDA_TreeMap));
  SCEDA_treemap_init(map, delete_key, delete_value, compare_key);
  return map;
}

void SCEDA_treemap_cleanup_aux(SCEDA_TreeMap *map, SCEDA_TreeMapElt *elt) {
  if(elt == SCEDA_treemap_nil(map)) {
    return;
  }
  SCEDA_treemap_cleanup_aux(map, elt->left);
  SCEDA_treemap_cleanup_aux(map, elt->right);
  if(map->delete_key != NULL) {
    map->delete_key(elt->key);
  }
  if(map->delete_value != NULL) {
    map->delete_value(elt->value);
  }
  free(elt);
}

void SCEDA_treemap_cleanup(SCEDA_TreeMap *map) {
  SCEDA_treemap_cleanup_aux(map, SCEDA_treemap_root(map));
  memset(map, 0, sizeof(SCEDA_TreeMap));
}

void SCEDA_treemap_delete(SCEDA_TreeMap *map) {
  SCEDA_treemap_cleanup(map);
  free(map);
}

void SCEDA_treemap_clear(SCEDA_TreeMap *map) {
  SCEDA_delete_fun delete_key = map->delete_key;
  SCEDA_delete_fun delete_value = map->delete_value;
  SCEDA_compare_fun compare_key = map->compare_key;
  SCEDA_treemap_cleanup(map);
  SCEDA_treemap_init(map, delete_key, delete_value, compare_key);
}

int SCEDA_treemap_contains_key(SCEDA_TreeMap *map, const void *key) {
  SCEDA_compare_fun compare = map->compare_key;

  SCEDA_TreeMapElt *x = SCEDA_treemap_root(map);
  while(x != SCEDA_treemap_nil(map)) {
    int res = compare(key, SCEDA_treemap_key(void *, x));

    if(res == 0) {
      return TRUE;
    } else if(res < 0) {
      x = x->left;
    } else {
      x = x->right;
    }
  }

  return FALSE;
}

void *SCEDA_treemap_get(SCEDA_TreeMap *map, const void *key) {
  SCEDA_compare_fun compare = map->compare_key;

  SCEDA_TreeMapElt *x = SCEDA_treemap_root(map);
  while(x != SCEDA_treemap_nil(map)) {
    int res = compare(key, SCEDA_treemap_key(void *, x));

    if(res == 0) {
      return SCEDA_treemap_value(void *, x);
    } else if(res < 0) {
      x = x->left;
    } else {
      x = x->right;
    }
  }

  return NULL;
}

int SCEDA_treemap_lookup(SCEDA_TreeMap *map, void **key, void **value) {
  SCEDA_compare_fun compare = map->compare_key;

  SCEDA_TreeMapElt *x = SCEDA_treemap_root(map);
  while(x != SCEDA_treemap_nil(map)) {
    int res = compare(*key, SCEDA_treemap_key(void *, x));

    if(res == 0) {
      *key = SCEDA_treemap_key(void *, x);
      *value = SCEDA_treemap_value(void *, x);
      return 0;
    } else if(res < 0) {
      x = x->left;
    } else {
      x = x->right;
    }
  }

  return -1;
}

static void SCEDA_treemap_left_rotate(SCEDA_TreeMap *map, SCEDA_TreeMapElt *x) {
  SCEDA_TreeMapElt *y = x->right;

  x->right = y->left;
  if(y->left != SCEDA_treemap_nil(map)) {
    y->left->parent = x;
  }

  y->parent = x->parent;
  if(x->parent == SCEDA_treemap_nil(map)) {
    map->root = y;
  } else if(x == x->parent->left) {
    x->parent->left = y;
  } else {
    x->parent->right = y;
  }

  y->left = x;
  x->parent = y;
}

static void SCEDA_treemap_right_rotate(SCEDA_TreeMap *map, SCEDA_TreeMapElt *y) {
  SCEDA_TreeMapElt *x = y->left;
  
  y->left = x->right;
  if(x->right != SCEDA_treemap_nil(map)) {
    x->right->parent = y;
  }

  x->parent = y->parent;
  if(y->parent == SCEDA_treemap_nil(map)) {
    map->root = x;
  } else if(y == y->parent->left) {
    y->parent->left = x;
  } else {
    y->parent->right = x;
  }

  x->right = y;
  y->parent = x;
}

int SCEDA_treemap_put(SCEDA_TreeMap *map, const void *key, const void *value, void **old_value) {
  SCEDA_compare_fun compare = map->compare_key;

  SCEDA_TreeMapElt *parent = SCEDA_treemap_root(map);
  int res = 0;
  while(parent != SCEDA_treemap_nil(map)) {
    res = compare(key, SCEDA_treemap_key(void *, parent));
    if(res == 0) {
      if(old_value != NULL) {
	*old_value = SCEDA_treemap_value(void *, parent);
      }
      parent->value = (void *)value;
      return 1;
    } else if(res < 0) {
      if(parent->left != SCEDA_treemap_nil(map)) {
	parent = parent->left;
      } else {
	break;
      }
    } else {
      if(parent->right != SCEDA_treemap_nil(map)) {
	parent = parent->right;
      } else {
	break;
      }
    }
  }

  SCEDA_TreeMapElt *element = (SCEDA_TreeMapElt *)safe_malloc(sizeof(SCEDA_TreeMapElt));
  element->key = (void *)key;
  element->value = (void *)value;
  element->parent = parent;
  element->left = SCEDA_treemap_nil(map);
  element->right = SCEDA_treemap_nil(map);
  element->color = RED;

  if(parent == SCEDA_treemap_nil(map)) {
    map->root = element;
  } else {
    if(res < 0) {
      parent->left = element;
    } else {
      parent->right = element;
    }
  }

  /* Red Black balancing */
  SCEDA_TreeMapElt *x = element;
  while((x != SCEDA_treemap_root(map)) && (x->parent->color == RED)) {
    if(x->parent == x->parent->parent->left) {
      SCEDA_TreeMapElt *y = x->parent->parent->right;
      if(y->color == RED) {
	x->parent->color = BLACK;
	y->color = BLACK;
	x->parent->parent->color = RED;
	x = x->parent->parent;
      } else {
	if(x == x->parent->right) {
	  x = x->parent;
	  SCEDA_treemap_left_rotate(map, x);
	} 
	x->parent->color = BLACK;
	x->parent->parent->color = RED;
	SCEDA_treemap_right_rotate(map, x->parent->parent);
      }
    } else {
      SCEDA_TreeMapElt *y = x->parent->parent->left;
      if(y->color == RED) {
	x->parent->color = BLACK;
	y->color = BLACK;
	x->parent->parent->color = RED;
	x = x->parent->parent;
      } else {
	if(x == x->parent->left) {
	  x = x->parent;
	  SCEDA_treemap_right_rotate(map, x);
	} 
	x->parent->color = BLACK;
	x->parent->parent->color = RED;
	SCEDA_treemap_left_rotate(map, x->parent->parent);
      }
    }
  }
  SCEDA_treemap_root(map)->color = BLACK;
  /* done */
  
  map->size++;

  return 0;
}

int SCEDA_treemap_remove(SCEDA_TreeMap *map, void **key, void **value) {
  SCEDA_compare_fun compare = map->compare_key;

  // find node in tree
  SCEDA_TreeMapElt *z = SCEDA_treemap_root(map);
  while(z != SCEDA_treemap_nil(map)) {
    int res = compare(*key, SCEDA_treemap_key(void *, z));
    if(res == 0) {
      break;
    } else if(res < 0) {
      z = z->left;
    } else {
      z = z->right;
    }
  }

  if(z == SCEDA_treemap_nil(map)) {
    return 1;
  }

  *key = SCEDA_treemap_key(void *, z);
  *value = SCEDA_treemap_value(void *, z);

  // find node which is going to be removed
  SCEDA_TreeMapElt *y;
  if((z->left == SCEDA_treemap_nil(map)) || (z->right == SCEDA_treemap_nil(map))) {
    y = z;
  } else {
    y = SCEDA_treemap_succ(map, z);
  }
  
  SCEDA_TreeMapElt *x;
  if(y->left != SCEDA_treemap_nil(map)) {
    x = y->left;
  } else {
    x = y->right;
  }

  x->parent = y->parent;

  if(y->parent == SCEDA_treemap_nil(map)) {
    map->root = x;
  } else {
    if(y == y->parent->left) {
      y->parent->left = x;
    } else {
      y->parent->right = x;
    }
  }

  if(y != z) {
    z->key = y->key;
    z->value = y->value;
  }

  int color = y->color;
  free(y);

  // Red Black balancing
  if(color == BLACK) {
    while((x != SCEDA_treemap_root(map)) && (x->color == BLACK)) {
      if(x == x->parent->left) {
	SCEDA_TreeMapElt *w = x->parent->right;
	if(w->color == RED) {
	  w->color = BLACK;
	  w->parent->color = RED;
	  SCEDA_treemap_left_rotate(map, x->parent);
	  w = x->parent->right;
	}
	if((w->left->color == BLACK) && (w->right->color == BLACK)) {
	  w->color = RED;
	  x = x->parent;
	} else {
	  if(w->right->color == BLACK) {
	    w->left->color = BLACK;
	    w->color = RED;
	    SCEDA_treemap_right_rotate(map, w);
	    w = x->parent->right;
	  }
	  w->color = x->parent->color;
	  x->parent->color = BLACK;
	  w->right->color = BLACK;
	  SCEDA_treemap_left_rotate(map, x->parent);
	  x = SCEDA_treemap_root(map);
	}
      } else {
	SCEDA_TreeMapElt *w = x->parent->left;
	if(w->color == RED) {
	  w->color = BLACK;
	  w->parent->color = RED;
	  SCEDA_treemap_right_rotate(map, x->parent);
	  w = x->parent->left;
	}
	if((w->right->color == BLACK) && (w->left->color == BLACK)) {
	  w->color = RED;
	  x = x->parent;
	} else {
	  if(w->left->color == BLACK) {
	    w->right->color = BLACK;
	    w->color = RED;
	    SCEDA_treemap_left_rotate(map, w);
	    w = x->parent->left;
	  }
	  w->color = x->parent->color;
	  x->parent->color = BLACK;
	  w->left->color = BLACK;
	  SCEDA_treemap_right_rotate(map, x->parent);
	  x = SCEDA_treemap_root(map);
	}
      }
    }
    x->color = BLACK;
  }

  map->size--;

  return 0;
}

void SCEDA_treemap_iterator_init(SCEDA_TreeMap *map, SCEDA_TreeMapIterator *iter) {
  iter->map = map;
  iter->elt = SCEDA_treemap_min(map, SCEDA_treemap_root(map));
}

int SCEDA_treemap_iterator_has_next(SCEDA_TreeMapIterator *iter) {
  return (iter->elt != SCEDA_treemap_nil(iter->map));
}

void *SCEDA_treemap_iterator_next(SCEDA_TreeMapIterator *iter, void **key) {
  SCEDA_TreeMapElt *elt = iter->elt;

  iter->elt = SCEDA_treemap_succ(iter->map, elt);

  *key = SCEDA_treemap_key(void *, elt);
  return SCEDA_treemap_value(void *, elt);
}

void SCEDA_treemap_iterator_cleanup(SCEDA_TreeMapIterator *iter) {
  memset(iter, 0, sizeof(SCEDA_TreeMapIterator));
}
