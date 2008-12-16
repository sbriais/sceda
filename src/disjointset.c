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
#include <stdlib.h>
#include <string.h>
#include "util.h"
#include "disjointset.h"

void SCEDA_disjoint_set_init(SCEDA_DisjointSet *set, SCEDA_delete_fun delete, void *data) {
  set->repr = NULL;
  set->rank = 0;
  set->data = data;
  set->delete = delete;
}

SCEDA_DisjointSet *SCEDA_disjoint_set_create(SCEDA_delete_fun delete, void *data) {
  SCEDA_DisjointSet *set = (SCEDA_DisjointSet *)safe_malloc(sizeof(SCEDA_DisjointSet));
  SCEDA_disjoint_set_init(set, delete, data);
  return set;
}

void SCEDA_disjoint_set_cleanup(SCEDA_DisjointSet *set) {
  if(set->delete != NULL) {
    set->delete(set->data);
  }
  memset(set, 0, sizeof(SCEDA_DisjointSet));
}

void SCEDA_disjoint_set_delete(SCEDA_DisjointSet *set) {
  SCEDA_disjoint_set_cleanup(set);
  free(set);
}

SCEDA_DisjointSet *SCEDA_disjoint_set_find(SCEDA_DisjointSet *set) {
  if(set->repr == NULL) {
    return set;
  } else {
    SCEDA_DisjointSet *repr = SCEDA_disjoint_set_find(set->repr);
    set->repr = repr;
    return repr;
  }
}

void SCEDA_disjoint_set_union(SCEDA_DisjointSet *set_x, SCEDA_DisjointSet *set_y) {
  SCEDA_DisjointSet *repr_x = SCEDA_disjoint_set_find(set_x);
  SCEDA_DisjointSet *repr_y = SCEDA_disjoint_set_find(set_y);
  if(repr_x == repr_y) {
    return;
  }
  if(repr_x->rank > repr_y->rank) {
    repr_y->repr = repr_x;
  } else {
    repr_x->repr = repr_y;
    if(repr_x->rank == repr_y->rank) {
      repr_y->rank++;
    }
  }
}

int SCEDA_disjoint_set_match(SCEDA_DisjointSet *set_x, SCEDA_DisjointSet *set_y) {
  SCEDA_DisjointSet *repr_x = SCEDA_disjoint_set_find(set_x);
  SCEDA_DisjointSet *repr_y = SCEDA_disjoint_set_find(set_y);
  return (repr_x == repr_y);
}
