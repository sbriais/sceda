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
#include "graph.h"
#include "graph_negcycle.h"
#include "hashmap.h"
#include "common.h"
#include "util.h"
#include "queue.h"
#include "boxed.h"

#define SCEDA_graph_neg_cycle(NAME,TYPE)				\
SCEDA_List *SCEDA_graph_neg_cycle_##NAME(SCEDA_Graph *g, TYPE (*cost)(SCEDA_Edge *e, void *ctxt), void *c_ctxt) { \
  int n = SCEDA_graph_vcount(g);					\
									\
  SCEDA_HashMap *dist = SCEDA_vertex_map_create((SCEDA_delete_fun)boxed_delete); \
  SCEDA_HashMap *incoming_edge = SCEDA_vertex_map_create(NULL);		\
  SCEDA_Queue *queue = SCEDA_queue_create(NULL);			\
  SCEDA_HashSet *in_queue = SCEDA_vertex_set_create();			\
									\
  {									\
    SCEDA_VerticesIterator vertices;					\
    SCEDA_vertices_iterator_init(g, &vertices);				\
    while(SCEDA_vertices_iterator_has_next(&vertices)) {		\
      SCEDA_Vertex *v = SCEDA_vertices_iterator_next(&vertices);	\
      safe_call(SCEDA_hashmap_put(dist, v, boxed_create(TYPE, 0), NULL)); \
      safe_call(SCEDA_queue_enqueue(queue, v));				\
      safe_call(SCEDA_hashset_add(in_queue, v));		    \
    }								    \
    SCEDA_vertices_iterator_cleanup(&vertices);			    \
  }								    \
  /** Mark end of phase by NULL */				    \
  safe_call(SCEDA_queue_enqueue(queue, NULL));			    \
								    \
  int i = 0;						\
  while((i < n) && (!SCEDA_queue_is_empty(queue))) {	\
    SCEDA_Vertex *u;					\
    safe_call(SCEDA_queue_dequeue(queue, (void **)&u)); \
    if(u == NULL) {					\
      i++;						\
      if(i < n) {					\
	safe_call(SCEDA_queue_enqueue(queue, NULL));	\
      }							\
      continue;						\
    }							\
							\
    safe_call(SCEDA_hashset_remove(in_queue, (void **)&u)); \
    boxed(TYPE) du = SCEDA_hashmap_get(dist, u);	    \
							    \
    SCEDA_OutEdgesIterator out_edges;				   \
    SCEDA_out_edges_iterator_init(u, &out_edges);			\
    while(SCEDA_out_edges_iterator_has_next(&out_edges)) {		   \
      SCEDA_Edge *e = SCEDA_out_edges_iterator_next(&out_edges);	\
      TYPE ce = cost(e, c_ctxt);					\
      SCEDA_Vertex *v = SCEDA_edge_target(e);				\
      boxed(TYPE) dv = SCEDA_hashmap_get(dist, v);			\
      if(boxed_get(du) + ce < boxed_get(dv)) {				\
	boxed_set(dv, boxed_get(du) + ce);				\
	SCEDA_hashmap_put(incoming_edge, v, e, NULL);			\
	if(!SCEDA_hashset_contains(in_queue, v)) {			\
	  safe_call(SCEDA_queue_enqueue(queue, v));			\
	  safe_call(SCEDA_hashset_add(in_queue, v));			\
	}								\
      }									\
    }									\
}									\
									\
  SCEDA_hashset_delete(in_queue);					\
  SCEDA_hashmap_delete(dist);						\
									\
  SCEDA_List *cycle_edges = SCEDA_list_create(NULL);			\
									\
  if((i < n) || (SCEDA_queue_is_empty(queue))) {			\
    SCEDA_queue_delete(queue);						\
    SCEDA_hashmap_delete(incoming_edge);				\
    return cycle_edges;							\
  }									\
									\
  SCEDA_Vertex *cycle;							\
  safe_call(SCEDA_queue_dequeue(queue, (void **)&cycle));		\
  /** cycle is a vertex reachable from a negative cycle */		\
  SCEDA_queue_delete(queue);						\
									\
  {									\
    /** find a vertex in the negative cycle */				\
    SCEDA_HashSet *cycle_elts = SCEDA_vertex_set_create();		\
    do {								\
      safe_call(SCEDA_hashset_add(cycle_elts, cycle));			\
      SCEDA_Edge *e = SCEDA_hashmap_get(incoming_edge, cycle);		\
      safe_ptr(e);							\
      cycle = SCEDA_edge_source(e);					\
    } while(!SCEDA_hashset_contains(cycle_elts, cycle));		\
    SCEDA_hashset_delete(cycle_elts);					\
  }									\
									\
  {									\
    SCEDA_Vertex *u = cycle;						\
    do {								\
      SCEDA_Edge *e = SCEDA_hashmap_get(incoming_edge, u);		\
      safe_ptr(e);							\
      safe_call(SCEDA_list_add(cycle_edges, e));			\
      u = SCEDA_edge_source(e);						\
    } while(u != cycle);						\
  }									\
									\
  SCEDA_hashmap_delete(incoming_edge);					\
									\
  return cycle_edges;							\
}

SCEDA_graph_neg_cycle(int,int)

SCEDA_graph_neg_cycle(long_double,long double)
