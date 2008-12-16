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
#include <string.h>
#include "treeset.h"
#include "util.h"

#define RED 0
#define BLACK 1

#define SCEDA_treeset_root(set) ((set)->root)
#define SCEDA_treeset_nil(set) (&((set)->nil))

#define RB_DEBUG 0

#if(RB_DEBUG)
#include <assert.h>

static int SCEDA_treeset_height_aux(SCEDA_TreeSet *set, SCEDA_TreeSetElt *x) {
  if(x == SCEDA_treeset_nil(set)) {
    return 0;
  }
  int l = SCEDA_treeset_height_aux(set, x->left);
  int r = SCEDA_treeset_height_aux(set, x->right);
  if(l > r) {
    return ++l;
  } else {
    return ++r;
  }
}

static int SCEDA_treeset_height(SCEDA_TreeSet *set) {
  return SCEDA_treeset_height_aux(set, SCEDA_treeset_root(set));
}

static void SCEDA_treeset_check_color(SCEDA_TreeSet *set, SCEDA_TreeSetElt *x) {
  if(x == SCEDA_treeset_nil(set)) {
    return;
  }
  if(x->color == RED) {
    if(x->left != SCEDA_treeset_nil(set)) {
      assert(x->left->color == BLACK);
    }
    if(x->right != SCEDA_treeset_nil(set)) {
      assert(x->right->color == BLACK);
    }
  }

  SCEDA_treeset_check_color(set, x->left);
  SCEDA_treeset_check_color(set, x->right);
}

static int SCEDA_treeset_check_black_height(SCEDA_TreeSet *set, SCEDA_TreeSetElt *x) {
  if(x == SCEDA_treeset_nil(set)) {
    return 0;
  }
  int l = SCEDA_treeset_check_black_height(set, x->left);
  int r = SCEDA_treeset_check_black_height(set, x->right);
  assert(l == r);
  if(x->color == BLACK) {
    return (1+l);
  } else {
    return l;
  }
}

static void SCEDA_treeset_check_order(SCEDA_TreeSet *set, SCEDA_TreeSetElt *x) {
  if(x == SCEDA_treeset_nil(set)) {
    return;
  }
  if(x->left != SCEDA_treeset_nil(set)) {
    assert(x->left->parent == x);
    assert(set->compare(SCEDA_treeset_data(void *, x->left), SCEDA_treeset_data(void *, x)) < 0);
  }
  if(x->right != SCEDA_treeset_nil(set)) {
    assert(x->right->parent == x);
    assert(set->compare(SCEDA_treeset_data(void *, x->right), SCEDA_treeset_data(void *, x)) > 0);
  }
  SCEDA_treeset_check_order(set, x->left);
  SCEDA_treeset_check_order(set, x->right);
}

static void SCEDA_treeset_check(SCEDA_TreeSet *set) {
  if(set->size == 0) {
    return;
  }
  assert(SCEDA_treeset_root(set)->color == BLACK);
  SCEDA_treeset_check_color(set, SCEDA_treeset_root(set));
  SCEDA_treeset_check_black_height(set, SCEDA_treeset_root(set));
  SCEDA_treeset_check_order(set, SCEDA_treeset_root(set));
}
#endif

static SCEDA_TreeSetElt *SCEDA_treeset_min(SCEDA_TreeSet *set, SCEDA_TreeSetElt *x) {
  while(x != SCEDA_treeset_nil(set)) {
    if(x->left == SCEDA_treeset_nil(set)) {
      break;
    }
    x = x->left;
  }
  return x;
}

static SCEDA_TreeSetElt *SCEDA_treeset_succ(SCEDA_TreeSet *set, SCEDA_TreeSetElt *x) {
  if(x->right != SCEDA_treeset_nil(set)) {
    // get min of right subtree
    x = x->right;
    return SCEDA_treeset_min(set, x);
  } else {
    // get root of left subtree
    SCEDA_TreeSetElt *y = x->parent;
    while((y != SCEDA_treeset_nil(set)) && (x == y->right)) {
      x = y;
      y = x->parent;
    }
    return y;
  }
}

void SCEDA_treeset_init(SCEDA_TreeSet *set, SCEDA_delete_fun delete, SCEDA_compare_fun compare) {
  set->size = 0;
  set->root = SCEDA_treeset_nil(set);
  set->delete = delete;
  set->compare = compare;
  set->nil.color = BLACK;
}

SCEDA_TreeSet *SCEDA_treeset_create(SCEDA_delete_fun delete, SCEDA_compare_fun compare) {
  SCEDA_TreeSet *set = (SCEDA_TreeSet *)safe_malloc(sizeof(SCEDA_TreeSet));
  SCEDA_treeset_init(set, delete, compare);
  return set;
}

void SCEDA_treeset_cleanup_aux(SCEDA_TreeSet *set, SCEDA_TreeSetElt *elt) {
  if(elt == SCEDA_treeset_nil(set)) {
    return;
  }
  SCEDA_treeset_cleanup_aux(set, elt->left);
  SCEDA_treeset_cleanup_aux(set, elt->right);
  if(set->delete != NULL) {
    set->delete(elt->data);
  }
  free(elt);
}

void SCEDA_treeset_cleanup(SCEDA_TreeSet *set) {
  SCEDA_treeset_cleanup_aux(set, SCEDA_treeset_root(set));
  memset(set, 0, sizeof(SCEDA_TreeSet));
}

void SCEDA_treeset_delete(SCEDA_TreeSet *set) {
  SCEDA_treeset_cleanup(set);
  free(set);
}

void SCEDA_treeset_clear(SCEDA_TreeSet *set) {
  SCEDA_delete_fun delete = set->delete;
  SCEDA_compare_fun compare = set->compare;
  SCEDA_treeset_cleanup(set);
  SCEDA_treeset_init(set, delete, compare);
}

int SCEDA_treeset_contains(SCEDA_TreeSet *set, const void *data) {
  SCEDA_compare_fun compare = set->compare;

  SCEDA_TreeSetElt *x = SCEDA_treeset_root(set);
  while(x != SCEDA_treeset_nil(set)) {
    int res = compare(data, SCEDA_treeset_data(void *, x));

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

int SCEDA_treeset_lookup(SCEDA_TreeSet *set, void **data) {
  SCEDA_compare_fun compare = set->compare;

  SCEDA_TreeSetElt *x = SCEDA_treeset_root(set);
  while(x != SCEDA_treeset_nil(set)) {
    int res = compare(*data, SCEDA_treeset_data(void *, x));

    if(res == 0) {
      *data = SCEDA_treeset_data(void *, x);
      return 0;
    } else if(res < 0) {
      x = x->left;
    } else {
      x = x->right;
    }
  }

  return -1;
}

static void SCEDA_treeset_left_rotate(SCEDA_TreeSet *set, SCEDA_TreeSetElt *x) {
  SCEDA_TreeSetElt *y = x->right;

  x->right = y->left;
  if(y->left != SCEDA_treeset_nil(set)) {
    y->left->parent = x;
  }

  y->parent = x->parent;
  if(x->parent == SCEDA_treeset_nil(set)) {
    set->root = y;
  } else if(x == x->parent->left) {
    x->parent->left = y;
  } else {
    x->parent->right = y;
  }

  y->left = x;
  x->parent = y;
}

static void SCEDA_treeset_right_rotate(SCEDA_TreeSet *set, SCEDA_TreeSetElt *y) {
  SCEDA_TreeSetElt *x = y->left;
  
  y->left = x->right;
  if(x->right != SCEDA_treeset_nil(set)) {
    x->right->parent = y;
  }

  x->parent = y->parent;
  if(y->parent == SCEDA_treeset_nil(set)) {
    set->root = x;
  } else if(y == y->parent->left) {
    y->parent->left = x;
  } else {
    y->parent->right = x;
  }

  x->right = y;
  y->parent = x;
}

int SCEDA_treeset_add(SCEDA_TreeSet *set, const void *data) {
  SCEDA_compare_fun compare = set->compare;

  SCEDA_TreeSetElt *parent = SCEDA_treeset_root(set);
  int res = 0;
  while(parent != SCEDA_treeset_nil(set)) {
    res = compare(data, SCEDA_treeset_data(void *, parent));
    if(res == 0) {
      return 1;
    } else if(res < 0) {
      if(parent->left != SCEDA_treeset_nil(set)) {
	parent = parent->left;
      } else {
	break;
      }
    } else {
      if(parent->right != SCEDA_treeset_nil(set)) {
	parent = parent->right;
      } else {
	break;
      }
    }
  }

  SCEDA_TreeSetElt *element = (SCEDA_TreeSetElt *)safe_malloc(sizeof(SCEDA_TreeSetElt));
  element->data = (void *)data;
  element->parent = parent;
  element->left = SCEDA_treeset_nil(set);
  element->right = SCEDA_treeset_nil(set);
  element->color = RED;

  if(parent == SCEDA_treeset_nil(set)) {
    set->root = element;
  } else {
    if(res < 0) {
      parent->left = element;
    } else {
      parent->right = element;
    }
  }

  /* Red Black balancing */
  SCEDA_TreeSetElt *x = element;
  while((x != SCEDA_treeset_root(set)) && (x->parent->color == RED)) {
    if(x->parent == x->parent->parent->left) {
      SCEDA_TreeSetElt *y = x->parent->parent->right;
      if(y->color == RED) {
	x->parent->color = BLACK;
	y->color = BLACK;
	x->parent->parent->color = RED;
	x = x->parent->parent;
      } else {
	if(x == x->parent->right) {
	  x = x->parent;
	  SCEDA_treeset_left_rotate(set, x);
	} 
	x->parent->color = BLACK;
	x->parent->parent->color = RED;
	SCEDA_treeset_right_rotate(set, x->parent->parent);
      }
    } else {
      SCEDA_TreeSetElt *y = x->parent->parent->left;
      if(y->color == RED) {
	x->parent->color = BLACK;
	y->color = BLACK;
	x->parent->parent->color = RED;
	x = x->parent->parent;
      } else {
	if(x == x->parent->left) {
	  x = x->parent;
	  SCEDA_treeset_right_rotate(set, x);
	} 
	x->parent->color = BLACK;
	x->parent->parent->color = RED;
	SCEDA_treeset_left_rotate(set, x->parent->parent);
      }
    }
  }
  SCEDA_treeset_root(set)->color = BLACK;
  /* done */
  
  set->size++;

#if(RB_DEBUG)
  SCEDA_treeset_check(set);
#endif

  return 0;
}

int SCEDA_treeset_remove(SCEDA_TreeSet *set, void **data) {
  SCEDA_compare_fun compare = set->compare;

  // find node in tree
  SCEDA_TreeSetElt *z = SCEDA_treeset_root(set);
  while(z != SCEDA_treeset_nil(set)) {
    int res = compare(*data, SCEDA_treeset_data(void *, z));
    if(res == 0) {
      break;
    } else if(res < 0) {
      z = z->left;
    } else {
      z = z->right;
    }
  }

  if(z == SCEDA_treeset_nil(set)) {
    return 1;
  }

  *data = SCEDA_treeset_data(void *, z);

  // find node which is going to be removed
  SCEDA_TreeSetElt *y;
  if((z->left == SCEDA_treeset_nil(set)) || (z->right == SCEDA_treeset_nil(set))) {
    y = z;
  } else {
    y = SCEDA_treeset_succ(set, z);
  }
  
  SCEDA_TreeSetElt *x;
  if(y->left != SCEDA_treeset_nil(set)) {
    x = y->left;
  } else {
    x = y->right;
  }

  x->parent = y->parent;

  if(y->parent == SCEDA_treeset_nil(set)) {
    set->root = x;
  } else {
    if(y == y->parent->left) {
      y->parent->left = x;
    } else {
      y->parent->right = x;
    }
  }

  if(y != z) {
    z->data = y->data;
  }

  int color = y->color;
  free(y);

  // Red Black balancing
  if(color == BLACK) {
    while((x != SCEDA_treeset_root(set)) && (x->color == BLACK)) {
      if(x == x->parent->left) {
	SCEDA_TreeSetElt *w = x->parent->right;
	if(w->color == RED) {
	  w->color = BLACK;
	  w->parent->color = RED;
	  SCEDA_treeset_left_rotate(set, x->parent);
	  w = x->parent->right;
	}
	if((w->left->color == BLACK) && (w->right->color == BLACK)) {
	  w->color = RED;
	  x = x->parent;
	} else {
	  if(w->right->color == BLACK) {
	    w->left->color = BLACK;
	    w->color = RED;
	    SCEDA_treeset_right_rotate(set, w);
	    w = x->parent->right;
	  }
	  w->color = x->parent->color;
	  x->parent->color = BLACK;
	  w->right->color = BLACK;
	  SCEDA_treeset_left_rotate(set, x->parent);
	  x = SCEDA_treeset_root(set);
	}
      } else {
	SCEDA_TreeSetElt *w = x->parent->left;
	if(w->color == RED) {
	  w->color = BLACK;
	  w->parent->color = RED;
	  SCEDA_treeset_right_rotate(set, x->parent);
	  w = x->parent->left;
	}
	if((w->right->color == BLACK) && (w->left->color == BLACK)) {
	  w->color = RED;
	  x = x->parent;
	} else {
	  if(w->left->color == BLACK) {
	    w->right->color = BLACK;
	    w->color = RED;
	    SCEDA_treeset_left_rotate(set, w);
	    w = x->parent->left;
	  }
	  w->color = x->parent->color;
	  x->parent->color = BLACK;
	  w->left->color = BLACK;
	  SCEDA_treeset_right_rotate(set, x->parent);
	  x = SCEDA_treeset_root(set);
	}
      }
    }
    x->color = BLACK;
  }

  set->size--;

#if(RB_DEBUG)
  SCEDA_treeset_check(set);
#endif

  return 0;
}

void SCEDA_treeset_iterator_init(SCEDA_TreeSet *set, SCEDA_TreeSetIterator *titer) {
  titer->set = set;
  titer->elt = SCEDA_treeset_min(set, SCEDA_treeset_root(set));
}

int SCEDA_treeset_iterator_has_next(SCEDA_TreeSetIterator *titer) {
  return (titer->elt != SCEDA_treeset_nil(titer->set));
}

void *SCEDA_treeset_iterator_next(SCEDA_TreeSetIterator *titer) {
  SCEDA_TreeSetElt *elt = titer->elt;

  titer->elt = SCEDA_treeset_succ(titer->set, elt);

  return SCEDA_treeset_data(void *, elt);
}

