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
#include "graph.h"
#include "graph_matching.h"
#include "util.h"
#include "queue.h"

#include "hashmap.h"

#define XPART 0
#define YPART 1

typedef enum { WHITE, GRAY, BLACK } SCEDA_VertexColor;

typedef struct {
  SCEDA_VertexColor visited; // for maximum matching
  SCEDA_Vertex *parent; // for maximum matching
  int matched; // for maximum matching
  int partition; // for maximum matching
} State;

static void state_delete(State *st) {
  free(st);
}

SCEDA_HashMap *SCEDA_graph_maximum_bipartite_matching(SCEDA_Graph *g, SCEDA_List *x_vert, SCEDA_List *y_vert) {
  SCEDA_HashMap *v_states = SCEDA_vertex_map_create((SCEDA_delete_fun)state_delete);

  SCEDA_HashMap *spouse = SCEDA_vertex_map_create(NULL);

#define init_state(v) State *st_##v = safe_malloc(sizeof(State)); SCEDA_hashmap_put(v_states, v, st_##v, NULL)
#define decl_state(v) State *st_##v = SCEDA_hashmap_get(v_states, v)
#define SCEDA_vertex_state(v) (st_##v)

#define SCEDA_vertex_set_parent(v, p) (SCEDA_vertex_state(v)->parent = p)
#define SCEDA_vertex_set_color(v, c) (SCEDA_vertex_state(v)->visited = c)
#define SCEDA_vertex_set_matched(v, b) (SCEDA_vertex_state(v)->matched = b)
#define SCEDA_vertex_set_partition(v, p) (SCEDA_vertex_state(v)->partition = p)

#define SCEDA_vertex_get_parent(v) (SCEDA_vertex_state(v)->parent)
#define SCEDA_vertex_get_color(v) (SCEDA_vertex_state(v)->visited)
#define SCEDA_vertex_get_matched(v) (SCEDA_vertex_state(v)->matched)
#define SCEDA_vertex_get_partition(v) (SCEDA_vertex_state(v)->partition)

#define SCEDA_vertex_set_spouse(v, w) (SCEDA_hashmap_put(spouse, v, w, NULL))
#define SCEDA_vertex_get_spouse(v) (SCEDA_hashmap_get(spouse, v))

  SCEDA_ListIterator vertice;
  SCEDA_Vertex *v;
  // initially, none of the vertice are matched

  SCEDA_list_iterator_init(x_vert, &vertice);
  while(SCEDA_list_iterator_has_next(&vertice)) {
    v = SCEDA_list_iterator_next(&vertice);
    init_state(v);
    SCEDA_vertex_set_matched(v, FALSE); // unmatched
    SCEDA_vertex_set_partition(v, XPART);
    SCEDA_vertex_set_spouse(v, NULL); // added for Dilworth
  }
  SCEDA_list_iterator_cleanup(&vertice);

  SCEDA_list_iterator_init(y_vert, &vertice);
  while(SCEDA_list_iterator_has_next(&vertice)) {
    v = SCEDA_list_iterator_next(&vertice);
    init_state(v);
    SCEDA_vertex_set_matched(v, FALSE); // unmatched
    SCEDA_vertex_set_partition(v, YPART);
    SCEDA_vertex_set_spouse(v, NULL);
  }
  SCEDA_list_iterator_cleanup(&vertice);

  //
  SCEDA_Queue *q = SCEDA_queue_create(NULL);
  SCEDA_Queue *path = SCEDA_queue_create(NULL);

  for(;;) {
    SCEDA_queue_init(q, NULL);

    // initialize breadthfirst traversal
    SCEDA_list_iterator_init(x_vert, &vertice);
    while(SCEDA_list_iterator_has_next(&vertice)) {
      v = SCEDA_list_iterator_next(&vertice);
      decl_state(v);
      if(!SCEDA_vertex_get_matched(v)) {
	SCEDA_vertex_set_color(v, GRAY);
	safe_call(SCEDA_queue_enqueue(q, v));
      } else {
	SCEDA_vertex_set_color(v, WHITE);
      }
      SCEDA_vertex_set_parent(v, NULL);
    }
    SCEDA_list_iterator_cleanup(&vertice);

    SCEDA_list_iterator_init(y_vert, &vertice);
    while(SCEDA_list_iterator_has_next(&vertice)) {
      v = SCEDA_list_iterator_next(&vertice);
      decl_state(v);
      SCEDA_vertex_set_color(v, WHITE);
      SCEDA_vertex_set_parent(v, NULL);
    }
    SCEDA_list_iterator_cleanup(&vertice);

    int couplage = XPART;

    while((SCEDA_queue_size(q) > 0) && ((couplage == XPART) || (SCEDA_queue_size(path) == 0))) {
      safe_call(SCEDA_queue_dequeue(q, (void **)&v));
      decl_state(v);
      couplage = SCEDA_vertex_get_partition(v);

      if(couplage == XPART) {
	// if v is in X then its neighbour are
	// the adjacent vertice not in the current matching
	
	// first proceed with the successors
	SCEDA_VertexSuccIterator v_succ;
	SCEDA_vertex_succ_iterator_init(v, &v_succ);
	while(SCEDA_vertex_succ_iterator_has_next(&v_succ)) {
	  SCEDA_Vertex *w = SCEDA_vertex_succ_iterator_next(&v_succ);
	  decl_state(w);
	  if((SCEDA_vertex_get_color(w) == WHITE) && (SCEDA_vertex_get_spouse(w) != v)) {
	    SCEDA_vertex_set_color(w, GRAY);
	    SCEDA_vertex_set_parent(w, v);
	    if(!SCEDA_vertex_get_matched(w)) {
	      // just found an unmatched vertex
	      safe_call(SCEDA_queue_enqueue(path, w));
	    }
	    safe_call(SCEDA_queue_enqueue(q, w));
	  }
	}
	SCEDA_vertex_succ_iterator_cleanup(&v_succ);
      } else {
	// if v is in Y then its unique neighbour, if it exists, is spouse(v)

	SCEDA_Vertex *w = SCEDA_vertex_get_spouse(v);
	if(w != NULL) {
	  decl_state(w);
	  if(SCEDA_vertex_get_color(w) == WHITE) {
	    SCEDA_vertex_set_color(w, GRAY);
	    SCEDA_vertex_set_parent(w, v);
	    safe_call(SCEDA_queue_enqueue(q, w));
	  }
	}
      }
      // end of traversal
      SCEDA_vertex_set_color(v, BLACK);
    }

    SCEDA_queue_cleanup(q); // clear the queue (it is possibly non empty)

    if(SCEDA_queue_size(path) == 0) {
      // no more augmenting path, we are done!
      break;
    }

    SCEDA_list_iterator_init(x_vert, &vertice);
    while(SCEDA_list_iterator_has_next(&vertice)) {
      v = SCEDA_list_iterator_next(&vertice);
      decl_state(v);
      SCEDA_vertex_set_color(v, WHITE);
    }
    SCEDA_list_iterator_cleanup(&vertice);

    SCEDA_list_iterator_init(y_vert, &vertice);
    while(SCEDA_list_iterator_has_next(&vertice)) {
      v = SCEDA_list_iterator_next(&vertice);
      decl_state(v);
      SCEDA_vertex_set_color(v, WHITE);
    }
    SCEDA_list_iterator_cleanup(&vertice);

    // here, we just found a maximal set of minimum augmenting path
    while(SCEDA_queue_size(path) > 0) {
      SCEDA_Vertex *endpath = NULL;
      SCEDA_queue_dequeue(path, (void **)&endpath);

      // check that the path is disjoint from previous ones
      SCEDA_Vertex *v = endpath;
      while(v != NULL) {
	decl_state(v);
	if(SCEDA_vertex_get_color(v) != WHITE) {
	  break;
	}
	SCEDA_vertex_set_color(v, BLACK);
	v = SCEDA_vertex_get_parent(v);
      }

      if(v == NULL) {
	decl_state(endpath);
	SCEDA_vertex_set_matched(endpath, TRUE);

	v = endpath;
	while(v != NULL) {
	  decl_state(v);
	  SCEDA_Vertex *w = SCEDA_vertex_get_parent(v);
	  decl_state(w);
	  SCEDA_vertex_set_spouse(v, w);
	  SCEDA_vertex_set_spouse(w, v); // added for Dilworth
	  v = SCEDA_vertex_get_parent(w);
	  if(v == NULL) {
	    SCEDA_vertex_set_matched(w, TRUE);
	  }
	}
      } 
    }
  }

  SCEDA_queue_delete(q);
  SCEDA_queue_delete(path);

  SCEDA_hashmap_delete(v_states);

  return spouse;

#undef SCEDA_vertex_state

#undef SCEDA_vertex_set_parent
#undef SCEDA_vertex_set_color
#undef SCEDA_vertex_set_matched
#undef SCEDA_vertex_set_partition

#undef SCEDA_vertex_get_parent
#undef SCEDA_vertex_get_color
#undef SCEDA_vertex_get_matched
#undef SCEDA_vertex_get_partition

#undef SCEDA_vertex_set_spouse
#undef SCEDA_vertex_get_spouse

#undef init_state
#undef decl_state
}

